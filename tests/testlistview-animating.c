#include <gtk/gtk.h>

#define AVERAGE 300
#define VARIANCE 200

static GtkWidget *
create_widget (gpointer unused)
{
  return gtk_label_new ("");
}

static void
bind_widget (GtkWidget *widget,
             gpointer   item,
             gpointer   unused)
{
  const char *message = g_object_get_data (item, "message");

  gtk_label_set_text (GTK_LABEL (widget), message);
}

static GtkWidget *
create_widget_for_listbox (gpointer item,
                           gpointer unused)
{
  GtkWidget *widget;

  widget = create_widget (unused);
  bind_widget (widget, item, unused);

  return widget;
}

static void
add (GListStore *store)
{
  static guint counter;
  GObject *o;
  char *message;
  guint pos;

  o = g_object_new (G_TYPE_OBJECT, NULL);
  message = g_strdup_printf ("Item %u", ++counter);
  g_object_set_data_full (o, "message", message, g_free);

  pos = g_random_int_range (0, g_list_model_get_n_items (G_LIST_MODEL (store)) + 1);
  g_list_store_insert (store, pos, o);
  g_object_unref (o);
}

static void
delete (GListStore *store)
{
  guint pos;

  pos = g_random_int_range (0, g_list_model_get_n_items (G_LIST_MODEL (store)));
  g_list_store_remove (store, pos);
}

static gboolean
do_stuff (gpointer store)
{
  if (g_random_int_range (AVERAGE - VARIANCE, AVERAGE + VARIANCE) < g_list_model_get_n_items (store))
    delete (store);
  else
    add (store);

  return G_SOURCE_CONTINUE;
}

int
main (int   argc,
      char *argv[])
{
  GtkWidget *win, *hbox, *vbox, *sw, *listview, *listbox, *label;
  GListStore *store;
  GListModel *toplevels;
  guint i;

  gtk_init ();

  win = gtk_window_new ();
  gtk_window_set_default_size (GTK_WINDOW (win), 400, 600);

  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
  gtk_window_set_child (GTK_WINDOW (win), hbox);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
  gtk_box_append (GTK_BOX (hbox), vbox);

  label = gtk_label_new ("GtkListView");
  gtk_box_append (GTK_BOX (vbox), label);

  sw = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_hexpand (sw, TRUE);
  gtk_widget_set_vexpand (sw, TRUE);
  gtk_box_append (GTK_BOX (vbox), sw);

  listview = gtk_list_view_new ();
  gtk_list_view_set_functions (GTK_LIST_VIEW (listview),
                               create_widget,
                               bind_widget,
                               NULL, NULL);
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (sw), listview);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
  gtk_box_append (GTK_BOX (hbox), vbox);

  label = gtk_label_new ("GtkListBox");
  gtk_box_append (GTK_BOX (vbox), label);

  sw = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_hexpand (sw, TRUE);
  gtk_widget_set_vexpand (sw, TRUE);
  gtk_box_append (GTK_BOX (vbox), sw);

  listbox = gtk_list_box_new ();
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (sw), listbox);

  store = g_list_store_new (G_TYPE_OBJECT);
  for (i = 0; i < AVERAGE; i++)
    add (store);
  gtk_list_view_set_model (GTK_LIST_VIEW (listview), G_LIST_MODEL (store));
  gtk_list_box_bind_model (GTK_LIST_BOX (listbox),
                           G_LIST_MODEL (store),
                           create_widget_for_listbox,
                           NULL, NULL);

  g_timeout_add (100, do_stuff, store);

  gtk_widget_show (win);

  toplevels = gtk_window_get_toplevels ();
  while (g_list_model_get_n_items (toplevels))
    g_main_context_iteration (NULL, TRUE);

  g_object_unref (store);

  return 0;
}