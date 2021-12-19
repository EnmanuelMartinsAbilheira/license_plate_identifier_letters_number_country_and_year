#define VC_DEBUG

typedef struct {
	unsigned char *data;
	int width, height;
	int channels;			// Binário/Cinzentos=1; RGB=3
	int levels;				// Binário=1; Cinzentos [1,255]; RGB [1,255]
	int bytesperline;		// width * channels
} IVC;


IVC *vc_image_new(int width, int height, int channels, int levels);
IVC *vc_image_free(IVC *image);

IVC *vc_read_image(char *filename);
int vc_write_image(char *filename, IVC *image);
