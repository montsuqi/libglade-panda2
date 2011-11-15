/*
panda-glade-convert.c
*/


#include <config.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glade/glade.h>
#include <glade/glade-private.h>
#include <libxml/tree.h>

#define CAST_BAD(x) ((gchar*)x)

static char *outfile = NULL;
static char *rootnode = NULL;

static GOptionEntry entries[] =
{
  { "out",'o',0,G_OPTION_ARG_STRING,&outfile,"output filename",NULL},
  { "rootnode",'r',0,G_OPTION_ARG_STRING,&rootnode,"root node name",NULL},
  { NULL}
};

static gboolean
is_child_name(char *name)
{
  if (!g_strcmp0(name,"child_name")) {
    return TRUE;
  }
  return FALSE;
}

static gboolean
have_child_name(GladeWidgetInfo *info)
{
  GList *list;

  for(list=info->attributes;list;list=list->next){
    GladeAttribute *attr = list->data;

    if (is_child_name(attr->name)) {
      return TRUE;
    }
  }
  return FALSE;
}

static char *packing_list[] = {
  "expand",
  "fill",
  "position",
  "x",
  "y",
  "left_attach",
  "right_attach",
  "top_attach",
  "bottom_attach",
  NULL
};

static gboolean
is_packing(char *name)
{
  int i;

  for(i=0;packing_list[i] != NULL; i++) {
    if (!g_strcmp0(packing_list[i],name)) {
      return TRUE;
    }
  } 
  return FALSE;
}

static gboolean
have_packing(GladeWidgetInfo *info)
{
  GList *list;

  for(list=info->attributes;list;list=list->next){
    GladeAttribute *attr = list->data;

    if (is_packing(attr->name)) {
      return TRUE;
    }
  }
  return FALSE;
}

static xmlNode *
dump_property(char *name,char *value)
{
  xmlNode *node;

  /*property check*/
  if (!g_strcmp0(name,"width")) {
    name = "width_request";
  }
  if (!g_strcmp0(name,"height")) {
    name = "height_request";
  }
  if (!g_strcmp0(name,"position")) {
    name = "window_position";
    if (!g_strcmp0(value,"GTK_WIN_POS_CENTER")) {
      value = "center";
    } else {
      value = "none";
    }
  }

  node = xmlNewNode(NULL,BAD_CAST("property"));
  xmlSetProp(node,BAD_CAST("name"),BAD_CAST(name));
  xmlNodeAddContent(node,BAD_CAST(value));
  return node;
}

static xmlNode *
dump_signal(GladeSignalInfo *signal)
{
  xmlNode * node;

  node = xmlNewNode(NULL,BAD_CAST("signal"));
  xmlSetProp(node,BAD_CAST("name"),BAD_CAST(signal->name));
  xmlSetProp(node,BAD_CAST("handler"),BAD_CAST(signal->handler));
  if (signal->after) {
    xmlSetProp(node,BAD_CAST("after"),BAD_CAST("yes"));
  }
  xmlSetProp(node,BAD_CAST("swapped"),BAD_CAST("no"));
  return node;  
}

static xmlNode *
_dump_widget(GladeWidgetInfo *info)
{
  xmlNode *node,*anode;
  GList *list;

  if (info == NULL) {
    return NULL;
  }

  node = xmlNewNode(NULL,BAD_CAST("object"));

  if (!g_strcmp0(info->class,"GtkProgressBar")) {
    info->class = "GtkPandaProgressBar";
  }

  xmlSetProp(node,BAD_CAST("class"),BAD_CAST(info->class));
  xmlSetProp(node,BAD_CAST("id"),BAD_CAST(info->name));

  /* property */
  for(list=info->attributes;list;list=list->next){
    GladeAttribute *attr = list->data;

    if (is_packing(attr->name)) {
      continue;
    }
    if (is_child_name(attr->name)) {
      continue;
    }
    if (!g_strcmp0(info->class,"GtkFrame") &&
        !g_strcmp0(attr->name,"label")) {
      GladeWidgetInfo *cinfo = g_new0(GladeWidgetInfo, 1);
      GladeAttribute *cattr = g_new0(GladeAttribute, 1);
      cinfo->parent = info;
      cinfo->class = "GtkLabel";
      cinfo->name = g_strdup_printf("%s_label",info->name);
      cattr->name = "label";
      cattr->value = attr->value;
      cinfo->attributes = g_list_append(cinfo->attributes,cattr);
      info->children = g_list_append(info->children,cinfo);
      continue;
    }
    anode = dump_property(attr->name,attr->value);
    xmlAddChild(node,anode);
  }
  /*signal*/
  for(list=info->signals;list;list=list->next){
    GladeSignalInfo *signal = list->data;
    xmlNode *snode;

    snode = dump_signal(signal);
    xmlAddChild(node,snode);
  }

  return node;
}

static xmlNode *
dump_widget(GladeWidgetInfo *info,gboolean istoplevel)
{
  GladeWidgetInfo *pinfo;
  xmlNode *object;
  GList *list;

  if (info == NULL) {
    return NULL;
  }
  pinfo = info->parent;

  object = _dump_widget(info);

  for(list=info->children;list;list=list->next){
    GladeWidgetInfo *cinfo = list->data;
    xmlNode *node;

    node = dump_widget(cinfo,FALSE);
    if (node != NULL) {
      xmlAddChild(object,node);
    }
  }

  if (!istoplevel) {
    xmlNode *cnode,*pnode;

    cnode = xmlNewNode(NULL,BAD_CAST("child"));
    xmlAddChild(cnode,object);
    /* add packing node*/
    if (have_packing(info)) {
      pnode = xmlNewNode(NULL,BAD_CAST("packing"));
      for(list=info->attributes;list;list=list->next){
        GladeAttribute *attr = list->data;

        if (is_packing(attr->name)) {
          xmlNode *node;

          node = dump_property(attr->name,attr->value);
          xmlAddChild(pnode,node);
        }
      }
      xmlAddChild(cnode,pnode);
    }
    /*child name*/
    if (have_child_name(info)) {
      for(list=info->attributes;list;list=list->next){
        GladeAttribute *attr = list->data;

        if (is_child_name(attr->name)) {
          if (!g_strcmp0(pinfo->class,"GtkNotebook")) {
            xmlSetProp(cnode,BAD_CAST("type"),BAD_CAST("tab"));
          }
        }
      }
    }
    /* for gtkframe */
    if (!g_strcmp0(info->parent->class,"GtkFrame") &&
        !g_strcmp0(info->class,"GtkLabel")) {
      xmlSetProp(cnode,BAD_CAST("type"),BAD_CAST("label"));
    }
    object = cnode;
  }
  return object;
}

static void 
convert(GladeWidgetTree *tree,char *rootnode)
{
  GladeWidgetInfo *info;
  xmlNode *root,*node;
  xmlDoc *doc;
  xmlChar *buf;
  int size;

  info = g_hash_table_lookup(tree->names,rootnode);
  if (info == NULL) {
    g_print("rootnode %s not found",rootnode);
    exit(1);
  }

  root = xmlNewNode(NULL,BAD_CAST("interface"));
  node = xmlNewNode(NULL,BAD_CAST("requires"));
  xmlSetProp(node,BAD_CAST("lib"),BAD_CAST("gtk+"));
  xmlSetProp(node,BAD_CAST("version"),BAD_CAST("2.24"));
  xmlAddChild(root,node);

  node = dump_widget(info,TRUE);
  if (node != NULL) {
    xmlAddChild(root,node);
  }

  doc = xmlNewDoc(BAD_CAST("1.0"));
  doc->standalone = FALSE;
  xmlDocSetRootElement(doc,root);

  xmlDocDumpFormatMemoryEnc(doc,&buf,&size,"utf-8",TRUE);
  if (outfile != NULL) {
    g_file_set_contents(outfile,CAST_BAD(buf),-1,NULL);
  } else {
    printf("%s",CAST_BAD(buf));
  }
}

int 
main (int argc, char **argv)
{
  GladeWidgetTree *tree;
  GOptionContext *ctx;
  gchar *buf1,*buf2;
  gsize size,sr,sw;
  GError *err = NULL;

  ctx = g_option_context_new(" <.glade>");
  g_option_context_add_main_entries(ctx, entries, NULL);
  if (!g_option_context_parse(ctx,&argc,&argv,NULL)) {
    g_print("option parsing failed\n");
    exit(1);
  }
  if (argc < 2) {
    g_print("%s",g_option_context_get_help(ctx,TRUE,NULL));
    exit(1);
  }

  gtk_init(&argc, &argv);
  glade_init();

  if (rootnode == NULL) {
    GRegex *reg;
    gchar *basename;

    basename = g_path_get_basename(argv[1]);
    reg = g_regex_new("\\..*",0,0,NULL);
    rootnode = g_regex_replace(reg,basename,-1,0,"",0,NULL);
    g_regex_unref(reg);
  }

  /* convert encoding */
  g_file_get_contents(argv[1], &buf1, &size, &err);
  if (err != NULL) {
    g_print("file read error %s\n",argv[1]);
    exit(1);
  }
  buf2 = g_convert(buf1,-1,"utf-8","euc-jp",&sr,&sw,NULL);
  if (err != NULL) {
    g_print("g_convert failure\n");
    exit(1);
  }
  g_free(buf1);

  tree = glade_widget_tree_parse_memory(buf2,strlen(buf2));
  if (tree == NULL) {
    g_print("fail glade_widget_tree_parse_file(%s)",argv[1]);
    exit(1);
  }
  convert(tree,rootnode);

  return 0;
}
