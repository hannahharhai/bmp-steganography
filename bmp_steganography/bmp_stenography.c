#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)
typedef struct BMP
{
    uint8_t type1;
    uint8_t type2;
    uint32_t size;
    uint16_t reserved_1;
    uint16_t reserved_2;
    uint32_t image_offset;
} BMP;

#pragma pack(1)
typedef struct DIB
{
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t color_planes;
    uint16_t bits_per_pixel;
    uint32_t compression_scheme;
    uint32_t image_size;
    uint32_t horizontal_resolution;
    uint32_t vertical_resolution;
    uint32_t colors_in_palette;
    uint32_t important_colors;
} DIB;

#pragma pack(1)
typedef struct pixel
{
    uint8_t blue_intensity;
    uint8_t green_intensity;
    uint8_t red_intensity;
} pixel;

int info(FILE *f, BMP b, DIB d)
{
    uint8_t format1 = 'B';
    uint8_t format2 = 'M';
    if (b.type1 == format1 && b.type2 == format2)
    {
        // printing BMP header
        printf("=== BMP Header ===\nType: %c%c\nSize: %d\nReserved 1: %d\nReserved 2: %d\nImage Offset: %d\n\n",
               b.type1, b.type2, b.size, b.reserved_1, b.reserved_2, b.image_offset);
    }
    else
    {
        printf("Error. This type of file format is not supported.\n");
        return -1;
    }

    // printing DIB header
    if (d.size == 40 && d.bits_per_pixel == 24)
    {
        printf("=== DIB Header ===\nSize: %d\nWidth: %d\nHeight: %d\n# color planes: %d\n# bits per pixel: %d\n"
               "Compression scheme: %d\nImage size: %d\nHorizontal resolution: %d\nVertical resolution: %d\n"
               "# colors in palette: %d\n# important colors: %d\n",
               d.size, d.width, d.height, d.color_planes, d.bits_per_pixel, d.compression_scheme,
               d.image_size, d.horizontal_resolution, d.vertical_resolution, d.colors_in_palette, d.important_colors);
    }
    else
    {
        printf("Error. This type of file format is not supported.\n");
        return -1;
    }
    return 1;
}

void reveal(FILE *f, BMP b, DIB d)
{
    fseek(f, b.image_offset, SEEK_SET);
    pixel pixel_array;
    for (int r = 0; r < d.height; r++)
    {
        for (int c = 0; c < d.width; c++)
        {
            fread(&pixel_array, sizeof(pixel), 1, f);

            // swapping blue bits
            unsigned blue_bottom = pixel_array.blue_intensity & 0x0F;
            unsigned blue_top = pixel_array.blue_intensity & 0xF0;
            uint8_t new_blue_pixel = (blue_bottom << 4 | blue_top >> 4);
            pixel_array.blue_intensity = new_blue_pixel;

            // swapping green bits
            unsigned green_bottom = pixel_array.green_intensity & 0x0F;
            unsigned green_top = pixel_array.green_intensity & 0xF0;
            uint8_t new_green_pixel = (green_bottom << 4 | green_top >> 4);
            pixel_array.green_intensity = new_green_pixel;

            // swapping red bits
            unsigned red_bottom = pixel_array.red_intensity & 0x0F;
            unsigned red_top = pixel_array.red_intensity & 0xF0;
            uint8_t new_red_pixel = (red_bottom << 4 | red_top >> 4);
            pixel_array.red_intensity = new_red_pixel;

            fseek(f, -1 * sizeof(pixel), SEEK_CUR);
            fwrite(&pixel_array, sizeof(pixel), 1, f);
        }
        int move_forward = (d.width * 3) % 4;
        if (move_forward != 0)
        {
            fseek(f, move_forward, SEEK_CUR);
        }
    }
}

void hide(FILE *f, BMP b, DIB d, FILE *f1, BMP b1, DIB d1)
{
    fseek(f, b.image_offset, SEEK_SET);
    fseek(f1, b1.image_offset, SEEK_SET);
    pixel pixel_array;
    pixel pixel_array1;

    if ((d.width == d1.width) && (d.height == d1.height))
    {
        for (int r = 0; r < d.height; r++)
        {
            for (int c = 0; c < d.width; c++)
            {
                fread(&pixel_array, sizeof(pixel), 1, f);
                // blue MSB of image1
                unsigned blue_top = pixel_array.blue_intensity & 0xF0;
                // green MSB of image1
                unsigned green_top = pixel_array.green_intensity & 0xF0;
                // red MSB of image1
                unsigned red_top = pixel_array.red_intensity & 0xF0;

                fread(&pixel_array1, sizeof(pixel), 1, f1);
                // blue MSB of image2
                unsigned blue_bottom = pixel_array1.blue_intensity & 0xF0;
                // green MSB of image2
                unsigned green_bottom = pixel_array1.green_intensity & 0xF0;
                // red MSB of image2
                unsigned red_bottom = pixel_array1.red_intensity & 0xF0;

                // combining bits
                uint8_t blue_result = (blue_top | blue_bottom >> 4);
                pixel_array.blue_intensity = blue_result;

                uint8_t green_result = (green_top | green_bottom >> 4);
                pixel_array.green_intensity = green_result;

                uint8_t red_result = (red_top | red_bottom >> 4);
                pixel_array.red_intensity = red_result;

                fseek(f, -1 * sizeof(pixel), SEEK_CUR);
                fwrite(&pixel_array, sizeof(pixel), 1, f);
            }
            int move_forward = (d.width * 3) % 4;
            int move_forward1 = (d1.width * 3) % 4;
            if (move_forward != 0)
            {
                fseek(f, move_forward, SEEK_CUR);
            }
            if (move_forward1 != 0) {
                fseek(f1, move_forward1, SEEK_CUR);
            }
        }
    }
    else
    {
        printf("Error. Images must be of same width and height.");
    }
}

int main(int argc, char *argv[])
{
    FILE *bmp_file = fopen(argv[2], "r+");
   
    BMP input_bmp;
    DIB input_dib;

    BMP input_bmp1;
    DIB input_dib1;

    fread(&input_bmp, sizeof(BMP), 1, bmp_file);
    fread(&input_dib, sizeof(DIB), 1, bmp_file);

    // prints headers
    if (strncmp(argv[1], "--info", 3) == 0)
    {
        info(bmp_file, input_bmp, input_dib);
    }
    // reveals secret immage
    if (strncmp(argv[1], "--reveal", 3) == 0)
    {    
        reveal(bmp_file, input_bmp, input_dib);
    }
    // hides image
    if (strncmp(argv[1], "--hide", 3) == 0)
    {
        FILE *bmp_file1 = fopen(argv[3], "r+");
        fread(&input_bmp1, sizeof(BMP), 1, bmp_file1);
        fread(&input_dib1, sizeof(DIB), 1, bmp_file1);

        hide(bmp_file, input_bmp, input_dib, bmp_file1, input_bmp1, input_dib1);

        fclose(bmp_file1);
    }

    fclose(bmp_file);
    return 0;
}
