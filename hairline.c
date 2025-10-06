#include <gtk/gtk.h>
#include <gst/gst.h>

typedef enum {
    NO_ERROR = 0,
    ERROR = -1
} errcode;

static inline void unref (gpointer object) {
    if (object != NULL)
        gst_object_unref (object);
}

/* Structure to contain all our information, so we can pass it around */
typedef struct _CustomData {
    GstElement *pipeline;    /* The main pipeline */
    GstElement *flip;        /* Handle of flip plugin */
    GstElement *invert;      /* Handle of invert plugin */
    GtkWidget  *sink_widget; /* Widget where video will be displayed */
} CustomData;

/* Use videoflip plugin to flip and rotate the image based on the current position
   0 = Identity, no rotation
   1 = Rotate clockwise 90 degrees
   2 = Rotate 180 degrees
   3 = Rotate counter-clockwise 90 degrees
   4 = Flip horizontally
   5 = Flip vertically
   6 = Flip across upper left/lower right diagonal
   7 = Flip across upper right/lower left diagonal
 */

/* Horizontal flip */
static void cb_flip_h (GtkButton * button, GstElement * flip) {
    gint method, new_method;

    g_object_get (flip, "method", &method, NULL);
    switch (method) {
        case 0:  new_method = 4; break;
        case 1:  new_method = 6; break;
        case 2:  new_method = 5; break;
        case 3:  new_method = 7; break;
        case 4:  new_method = 0; break;
        case 5:  new_method = 2; break;
        case 6:  new_method = 1; break;
        case 7:  new_method = 3; break;
        default: new_method = 0;
    }
    g_object_set (flip, "method", new_method, NULL);
}

/* Vertical flip */
static void cb_flip_v (GtkButton * button, GstElement * flip) {
    gint method, new_method;

    g_object_get (flip, "method", &method, NULL);
    switch (method) {
        case 0:  new_method = 5; break;
        case 1:  new_method = 7; break;
        case 2:  new_method = 4; break;
        case 3:  new_method = 6; break;
        case 4:  new_method = 2; break;
        case 5:  new_method = 0; break;
        case 6:  new_method = 3; break;
        case 7:  new_method = 1; break;
        default: new_method = 0;
    }
    g_object_set (flip, "method", new_method, NULL);
}

/* Rotate clockwise 90 degrees */
static void cb_rotate_cw (GtkButton * button, GstElement * flip) {
    gint method, new_method;

    g_object_get (flip, "method", &method, NULL);
    switch (method) {
        case 0:  new_method = 1; break;
        case 1:  new_method = 2; break;
        case 2:  new_method = 3; break;
        case 3:  new_method = 0; break;
        case 4:  new_method = 7; break;
        case 5:  new_method = 6; break;
        case 6:  new_method = 4; break;
        case 7:  new_method = 5; break;
        default: new_method = 0;
    }
    g_object_set (flip, "method", new_method, NULL);
}

/* Rotate anti-clockwise 90 degrees */
static void cb_rotate_acw (GtkButton * button, GstElement * flip) {
    gint method, new_method;

    g_object_get (flip, "method", &method, NULL);
    switch (method) {
        case 0:  new_method = 3; break;
        case 1:  new_method = 0; break;
        case 2:  new_method = 1; break;
        case 3:  new_method = 2; break;
        case 4:  new_method = 6; break;
        case 5:  new_method = 7; break;
        case 6:  new_method = 5; break;
        case 7:  new_method = 4; break;
        default: new_method = 0;
    }
    g_object_set (flip, "method", new_method, NULL);
}

/* Invert colors using coloreffects plugin preset #3
   0 = Do nothing
   1 = Fake heat camera toning
   2 = Sepia toning
   3 = Invert and slightly shade to blue
   4 = Cross processing toning
   5 = Yellow foreground Blue background color filter
 */
static void cb_invert (GtkButton * button, GstElement * invert) {
    gint preset, new_preset;

    g_object_get (invert, "preset", &preset, NULL);
    switch (preset) {
        case 0:  new_preset=3; break;
        case 3:  new_preset=0; break;
        default: new_preset=0;
    }
    g_object_set (invert, "preset", new_preset, NULL);
}

/* This function is called when the main window is closed */
static void cb_delete_event (GtkWidget * widget, GdkEvent * event, GstElement * pipeline) {
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gtk_main_quit ();
}

/* This creates all the GTK+ widgets that compose our application, and registers the callbacks */
static void create_ui (CustomData * data) {
    GtkWidget *main_window;     /* The uppermost window, containing all other windows */
    GtkWidget *main_box;        /* VBox to hold main_hbox and the controls */
    GtkWidget *main_hbox;       /* HBox to hold the video sink */
    GtkWidget *controls;        /* HBox to hold the buttons */
    GtkWidget *button_flip_h,    *button_flip_v,
              *button_rotate_cw, *button_rotate_acw,
              *button_invert;   /* Buttons */

    main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (G_OBJECT (main_window), "delete-event", G_CALLBACK (cb_delete_event), data->pipeline);

    button_flip_h     = gtk_button_new_with_label ("\u21C6");
    button_flip_v     = gtk_button_new_with_label ("\u21F5");
    button_rotate_cw  = gtk_button_new_with_label ("\u2B6E");
    button_rotate_acw = gtk_button_new_with_label ("\u2B6F");
    button_invert     = gtk_button_new_with_label ("\u25D0");

    g_signal_connect (G_OBJECT (button_flip_h),     "clicked", G_CALLBACK (cb_flip_h),     data->flip);
    g_signal_connect (G_OBJECT (button_flip_v),     "clicked", G_CALLBACK (cb_flip_v),     data->flip);
    g_signal_connect (G_OBJECT (button_rotate_cw),  "clicked", G_CALLBACK (cb_rotate_cw),  data->flip);
    g_signal_connect (G_OBJECT (button_rotate_acw), "clicked", G_CALLBACK (cb_rotate_acw), data->flip);
    g_signal_connect (G_OBJECT (button_invert),     "clicked", G_CALLBACK (cb_invert),     data->invert);

    controls = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (controls), button_flip_h,     FALSE, FALSE, 2);
    gtk_box_pack_start (GTK_BOX (controls), button_flip_v,     FALSE, FALSE, 2);
    gtk_box_pack_start (GTK_BOX (controls), button_rotate_cw,  FALSE, FALSE, 2);
    gtk_box_pack_start (GTK_BOX (controls), button_rotate_acw, FALSE, FALSE, 2);
    gtk_box_pack_start (GTK_BOX (controls), button_invert,     FALSE, FALSE, 2);
    gtk_widget_set_halign(controls, GTK_ALIGN_CENTER);

    main_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (main_hbox), data->sink_widget, TRUE, TRUE, 0);

    main_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start (GTK_BOX (main_box), main_hbox, TRUE,  TRUE,  0);
    gtk_box_pack_start (GTK_BOX (main_box), controls,  FALSE, FALSE, 0);
    gtk_container_add (GTK_CONTAINER (main_window), main_box);
    gtk_window_set_default_size (GTK_WINDOW (main_window), 1280, 720);

    gtk_widget_show_all (main_window);
}

/* This function is called when an error message is posted on the bus */
static void cb_error (GstBus * bus, GstMessage * msg, GstElement * pipeline) {
    GError *err;
    gchar  *debug_info;

    /* Print error details on the screen */
    gst_message_parse_error (msg, &err, &debug_info);
    g_printerr ("Error received from element %s: %s\n",
            GST_OBJECT_NAME (msg->src), err->message);
    g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
    g_clear_error (&err);
    g_free (debug_info);

    /* Stop the pipeline */
    gst_element_set_state (pipeline, GST_STATE_NULL);
    /* Restart the pipeline */
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
}

/* This function is called when an End-Of-Stream message is posted on the bus.
 * We just set the pipeline to READY (which stops playback) */
static void cb_eos (GstBus * bus, GstMessage * msg, GstElement * pipeline) {
    g_print ("End-Of-Stream reached.\n");

    /* Stop the pipeline */
    gst_element_set_state (pipeline, GST_STATE_NULL);
    /* Restart the pipeline */
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
}

int main (int argc, char *argv[]) {
    errcode err = NO_ERROR;
    CustomData data;
    GstStateChangeReturn ret;
    gboolean link;
    GstBus *bus;
    GstElement *camera, *flip, *invert, *convert1, *convert2, *h264enc, *h264dec,
        *videosink, *gtkglsink, *pipeline;

    /* Initialize GTK */
    gtk_init (&argc, &argv);

    /* Initialize GStreamer */
    gst_init (&argc, &argv);

    /* Initialize our data structure */
    memset (&data, 0, sizeof (data));

    camera    = gst_element_factory_make ("v4l2src",      "camera");
    flip      = gst_element_factory_make ("videoflip",    "flip");
    convert1  = gst_element_factory_make ("videoconvert", "convert1");
    invert    = gst_element_factory_make ("coloreffects", "invert");
    convert2  = gst_element_factory_make ("videoconvert", "convert2");
    videosink = gst_element_factory_make ("glsinkbin",    "glsinkbin");
    gtkglsink = gst_element_factory_make ("gtkglsink",    "gtkglsink");

    /* Here we create the GTK Sink element which will provide us with a GTK widget where
     * GStreamer will render the video at and we can add to our UI.
     * Try to create the OpenGL version of the video sink, and fallback if that fails */
    if (gtkglsink != NULL && videosink != NULL) {
        g_print ("Successfully created GTK GL Sink\n");

        g_object_set (videosink, "sink", gtkglsink, NULL);

        /* The gtkglsink creates the gtk widget for us. This is accessible through a property.
         * So we get it and use it later to add it to our gui. */
        g_object_get (gtkglsink, "widget", &data.sink_widget, NULL);
    } else {
        unref (gtkglsink);
        unref (videosink);

        g_printerr ("Could not create gtkglsink, falling back to gtksink.\n");

        videosink = gst_element_factory_make ("gtksink", "gtksink");
        g_object_get (videosink, "widget", &data.sink_widget, NULL);
    }


    pipeline  = gst_pipeline_new ("main-pipeline");
    if (!camera || !flip || !convert1 || !invert || !convert2 ||
        !pipeline || !videosink) {
        g_printerr ("Not all elements could be created.\n");
        err = ERROR;
        goto exit;
    }

#if H264_ENCRYPTION == 1
    h264enc   = gst_element_factory_make ("openh264enc",  "h264enc");
    h264dec   = gst_element_factory_make ("openh264dec",  "h264dec");
    if (!h264enc || !h264dec) {
        g_printerr ("Not all elements could be created.\n");
        err = ERROR;
        goto exit;
    }

    gst_bin_add_many (GST_BIN (pipeline), camera, flip, convert1, invert, convert2,
            h264enc, h264dec, videosink, NULL);
    link = gst_element_link_many (camera, flip, convert1, invert, convert2,
            h264enc, h264dec, videosink, NULL);
#else
    gst_bin_add_many (GST_BIN (pipeline), camera, flip, convert1, invert, convert2,
            videosink, NULL);
    link = gst_element_link_many (camera, flip, convert1, invert, convert2,
            videosink, NULL);
#endif

    if (!link) {
        g_printerr ("Elements could not be linked.\n");       
        err = ERROR;
        goto exit;
    }

    /* Create the elements */
    data.pipeline = pipeline;
    data.flip     = flip;
    data.invert   = invert;

    /* Create the GUI */
    create_ui (&data);

    /* Instruct the bus to emit signals for each received message, and connect to the interesting signals */
    bus = gst_element_get_bus (pipeline);
    gst_bus_add_signal_watch (bus);
    g_signal_connect (G_OBJECT (bus), "message::error", (GCallback) cb_error, pipeline);
    g_signal_connect (G_OBJECT (bus), "message::eos",   (GCallback) cb_eos,   pipeline);
    unref (bus);

    /* Start playing */
    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        err = ERROR;
        goto exit;
    }

    /* Start the GTK main loop. We will not regain control until gtk_main_quit is called. */
    gtk_main ();

exit:
    /* Stop pipeline */
    gst_element_set_state (data.pipeline, GST_STATE_NULL);

    /* Free resources */
    unref (pipeline);
    unref (gtkglsink);
    unref (videosink);
    unref (convert2);
    unref (invert);
    unref (convert1);
    unref (flip);
    unref (camera);
#if H264_ENCRYPTION == 1
    unref (h264dec);
    unref (h264enc);
#endif

    return err;
}
