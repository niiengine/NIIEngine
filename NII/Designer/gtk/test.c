#include <gtk/gtk.h>

/* 一个回调函数 */
gint delete_event( GtkWidget *widget, GdkEvent  *event, gpointer   data )
{
    gtk_main_quit ();
    return TRUE;
}

int main(int argc,char* argv[])
{
	GtkBuilder *builder;
	GtkWidget *window;
	gtk_init (&argc,&argv);
	builder=gtk_builder_new();
	gtk_builder_add_from_file(builder,"nii.glade",NULL);
    window=GTK_WIDGET(gtk_builder_get_object(builder,"main"));
    /* 在这里我们为 delete_event 设置了一个处理函数来立即退出 GTK。*/
    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (delete_event), NULL);	
	g_object_unref(G_OBJECT(builder));
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}