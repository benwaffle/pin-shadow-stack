#include <gtk/gtk.h>

gboolean quit(gpointer p){
	gtk_main_quit();
	return FALSE;
}

int main(int argc, char **argv) {
	gtk_init(&argc, &argv);
	puts("start");
	g_timeout_add(1000, quit, NULL);
	gtk_main();
	puts("end");
}
