#include <gtk/gtk.h>

gboolean quit(gpointer p){
	gtk_main_quit();
	return FALSE;
}

int main(int argc, char **argv) {
	puts("start");
	gtk_init(&argc, &argv);
	g_timeout_add(1000, quit, NULL);
	gtk_main();
	puts("end");
}
