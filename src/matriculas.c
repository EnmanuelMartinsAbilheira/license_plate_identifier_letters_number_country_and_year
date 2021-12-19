#include <stdio.h>
#include "vc.c"
// #define VC_DEBUG


int vc_rgb_to_gray(IVC *src, IVC *dst) {

    unsigned char *datasrc = (unsigned char *) src->data;
    int bytesperline_src = src->width * src->channels;
    int channels_src = src->channels;
    unsigned char *datadst = (unsigned char *) dst->data;
    int bytesperline_dst = dst->width * dst->channels;
    int channels_dst = dst->channels;
    int width = src->width;
    int height = src->height;
    int x, y;
    long int pos_src, pos_dst;
    float rf, gf, bf;

    // Verificação de erros
    if((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
    if((src->width != dst->width) || (src->height != dst->height)) return 0;
    if((src->channels != 3) || (dst->channels != 1)) return 0;

    for(y = 0; y < height; y++) {
        for(x = 0; x < width; x++) {
            pos_src = y * bytesperline_src + x * channels_src;
            pos_dst = y * bytesperline_dst + x * channels_dst;

            rf = (float) datasrc[pos_src];
            gf = (float) datasrc[pos_src + 1];
            bf = (float) datasrc[pos_src + 2];

            datadst[pos_dst] = (unsigned char) ((rf * 0.299)+(gf * 0.587)+(bf * 0.114));
        }
    }
    return 1;
}


// -----


int vc_scale_gray_to_rgb(IVC *src, IVC *dst)
{
    unsigned char *datasrc = (unsigned char *)src->data;
    int bytesperline_src = src->bytesperline;
    int channels_src = src->channels;
    unsigned char *datadst = (unsigned char *)dst->data;
    int bytesperline_dst = dst->bytesperline;
    int channels_dst = dst->channels;
    int width = src->width;
    int height = src->height;
    int x, y;
    long int pos_src, pos_dst;

    // Verificação de erros
    if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
    if ((src->width != dst->width) || (src->height != dst->height)) return 0;
    if ((src->channels != 1) || (dst->channels != 3)) return 0;

    for (y = 0; y<height; y++)
    {
        for (x = 0; x<width; x++)
        {
            pos_src = y * bytesperline_src + x * channels_src;
            pos_dst = y * bytesperline_dst + x * channels_dst;

            // Blue to Cyan
            if (datasrc[pos_src] < 64)
            {
                datadst[pos_dst] = 0;
                datadst[pos_dst + 1] = datasrc[pos_src] * 4;
                datadst[pos_dst + 2] = 255;
            }
            // Cyan to Green
            else if (datasrc[pos_src] < 128)
            {
                datadst[pos_dst] = 0;
                datadst[pos_dst + 1] = 255;
                datadst[pos_dst + 2] = 255 - (datasrc[pos_src] - 64) * 4;
            }
            // Green to Yellow
            else if (datasrc[pos_src] < 192)
            {
                datadst[pos_dst] = (datasrc[pos_src] - 128) * 4;
                datadst[pos_dst + 1] = 255;
                datadst[pos_dst + 2] = 0;
            }
            // Yellow to Red
            else
            {
                datadst[pos_dst] = 255;
                datadst[pos_dst + 1] = 255 - (datasrc[pos_src] - 192) * 4;
                datadst[pos_dst + 2] = 0;
            }
        }
    }

    return 1;
}
// Converter de Gray para Binário (threshold automático Midpoint)
int vc_gray_to_binary_midpoint(IVC *src, IVC *dst, int kernel)
{
    unsigned char *datasrc = (unsigned char *)src->data;
    unsigned char *datadst = (unsigned char *)dst->data;
    int width = src->width;
    int height = src->height;
    int bytesperline = src->bytesperline;
    int channels = src->channels;
    int x, y, kx, ky;
    int offset = (kernel - 1) / 2; //(int) floor(((double) kernel) / 2.0);
    int max, min;
    long int pos, posk;
    unsigned char threshold;

    // Verificação de erros
    if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
    if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels)) return 0;
    if (channels != 1) return 0;

    for (y = 0; y<height; y++)
    {
        for (x = 0; x<width; x++)
        {
            pos = y * bytesperline + x * channels;

            max = 0;
            min = 255;

            // NxM Vizinhos
            for (ky = -offset; ky <= offset; ky++)
            {
                for (kx = -offset; kx <= offset; kx++)
                {
                    if ((y + ky >= 0) && (y + ky < height) && (x + kx >= 0) && (x + kx < width))
                    {
                        posk = (y + ky) * bytesperline + (x + kx) * channels;

                        if (datasrc[posk] > max) max = datasrc[posk];
                        if (datasrc[posk] < min) min = datasrc[posk];
                    }
                }
            }

            threshold = (unsigned char)((float)(max + min) / (float)2);

            if (datasrc[pos] > threshold) datadst[pos] = 255;
            else datadst[pos] = 0;
        }
    }

    return 1;
}
// Converter de Gray para Binário (threshold manual)
int vc_gray_to_binary(IVC *srcdst, int threshold)
{
    unsigned char *data = (unsigned char *)srcdst->data;
    int width = srcdst->width;
    int height = srcdst->height;
    int bytesperline = srcdst->bytesperline;
    int channels = srcdst->channels;
    int x, y;
    long int pos;

    // Verificação de erros
    if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL)) return 0;
    if (channels != 1) return 0;

    srcdst->levels = 1;

    for (y = 0; y<height; y++)
    {
        for (x = 0; x<width; x++)
        {
            pos = y * bytesperline + x * channels;

            if (data[pos] <= (unsigned char)threshold) data[pos] = 0;
            else data[pos] = 255;
        }
    }

    return 1;
}
// Dilate
int vc_binary_dilate(IVC *src, IVC *dst, int kernel)
{
    // Verificação de erros
    if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
    if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels)) return 0;
    if ((src->channels != 1) || (dst->channels != 1)) return 0;

    int x, y, kx, ky;
    int offset = (kernel - 1) / 2; //(int) floor(((double) kernel) / 2.0); Rebordo/Margem do kernel
    long int pos, posk;
    int dilate;

    for (y = 0; y < src->height; y++)
    {
        for (x = 0; x < src->width; x++)
        {
            pos = y * src->bytesperline + x * src->channels;

            // Re-inicia o valor
            dilate = 0;

            // Kernel - NxM Vizinhos. Procura na vizinhaça do pixel na "pos" os pixeis do primeiro plano.
            for (ky = -offset; ky <= offset; ky++)
            {
                for (kx = -offset; kx <= offset; kx++)
                {
                    if ((y + ky >= 0) && (y + ky < src->height) && (x + kx >= 0) && (x + kx < src->width))
                    {
                        posk = (y + ky) * src->bytesperline + (x + kx) * src->channels;

                        if (src->data[posk] == 1) // Se igual 1, corresponde a pixel do primeiro plano
                        {
                            dilate = 1;
                            continue; // Quebra o ciclo for caso já tenha encontrado um pixel do primeiro plano
                        }
                    }
                }
                if (dilate == 1) continue; // Quebra o ciclo for caso já tenha encontrado um pixel do primeiro plano
            }

            if (dilate == 1)
                dst->data[pos] = 1;
            else
                dst->data[pos] = 0;
        }
    }

    return 1;
}

int vc_gray_histogram_show(IVC *src,IVC *dst){
    int x,y,pos,gray_level[256]={0},max=0,bpl=dst->bytesperline;

    if ((src->channels != 1) || (dst->channels != 1)) return 0;

    for (y = 0; y<src->height; y++)
    {
        for (x = 0; x<src->width; x++)
        {
            pos = y * src->bytesperline + x;
            gray_level[src->data[pos]]+=1;
            if(gray_level[src->data[pos]]>max) max=gray_level[src->data[pos]];
        }
    }

    for (y = 0; y<dst->height; y++)
    {
        for (x = 0; x<dst->width; x++)
        {
            pos =((dst->height-1-y)*bpl) + x;
            if(gray_level[x]*dst->height/max>y) dst->data[pos]=1;

        }
    }    




    return 0;
}

int vc_perfil_columna(IVC *src, IVC *dst) {

    int x, y, soma, pos_src, pos_dst;

    for (x = 0; x < src->width; x++)
    {
        soma = 0;

        for (y = 0; y < src->height; y++)
        {
            pos_src = y * src->bytesperline + x;

            soma += src->data[pos_src];
        }

        pos_dst = (dst->height-1-(soma / y)) * dst->bytesperline +x;
        dst->data[pos_dst] = 1;
    }


    return 0;
}

int vc_perfil_rows(IVC *src, IVC *dst) {

    int x, y, soma, pos_src, pos_dst;

    for (y = 0; y < src->height; y++)
    {
        soma = 0;

         for (x = 0; x < src->width; x++)
        {
            pos_src = y * src->bytesperline + x;

            soma += src->data[pos_src];
        }

        pos_dst = y * dst->bytesperline +(soma/x);
        dst->data[pos_dst] = 1;
    }


    return 0;
}

int main(int argc, char const *argv[]) {


    IVC *image, *gray, *hist, *new_image, *new_image2;

    // image = vc_read_image("img/Imagem01-8449FS.ppm"); // quando tive ima imagem binaria pbm usar esto para carregar
    image = vc_read_image("img/tire.pgm");

    if (image == NULL)
    {
        printf("ERROR -> vc_read_image():\n\tFile not found!\n");
        getchar();
        return 0;
    }

    gray = vc_image_new(image->width, image->height, 1, image->levels);
    hist = vc_image_new(256,256,1,1);

    // vc_rgb_to_gray(image, gray);
    // vc_write_image("output/Imagem01-8449FS.pgm", gray);
    vc_gray_histogram_show(image,hist);
    vc_write_image("output/histograma.pbm", hist);

    new_image = vc_image_new(image->width, 256, 1, 1);
    vc_perfil_columna(image, new_image);
    vc_write_image("output/perfil_columna.pbm", new_image);
    
    new_image2 = vc_image_new(256, image->height, 1, 1);
    vc_perfil_rows(image, new_image2);
    vc_write_image("output/perfil_rows.pbm", new_image2);
    
    return 0;

}

