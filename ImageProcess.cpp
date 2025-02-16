#include "ImageProcess.h"

#include "dirent/include/dirent.h"
#include "Bitmap.h"
#include "ThreeDTree.h"

#include <stdio.h>
#include <math.h>
#include <fstream>
#include <sstream>
#include <iomanip>

void imageprocess::preprocessImage(int width, int height)
{
    DIR *dir;
    struct dirent *ent;

    char *path = "images/unprocessed/";
    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_name[0] != '.')
            {
                unsigned char *data;
                int w,
                    h;

                char *file_path = new char[strlen(path) + ent->d_namlen + 1];
                strcpy(file_path, path);
                strcat(file_path, ent->d_name);

                if ((data = readBMP(file_path, w, h)) == NULL)
                {
                    continue;
                }

                int width_scale = int(w) / width;
                int height_scale = int(h) / height;
                int scale = min(width_scale, height_scale);

                int new_width = width * scale;
                int new_height = height * scale;

                // do crop
                unsigned char *temp_data = crop(data, w, h, new_width, new_height);
                delete[] data;
                data = temp_data;

                // do resize
                temp_data = resize(data, new_width, new_height, scale, false);
                delete[] data;
                data = temp_data;

                char *new_path = "images/processed/";
                char *new_file_path = new char[strlen(new_path) + 18];
                strcpy(new_file_path, new_path);
                unsigned char *colors = new unsigned char[3];
                averageColor(data, width, height, colors);

                std::stringstream ss;
                for (int i = 0; i < 3; i++)
                {
                    ss << std::setfill('0') << std::setw(3) << int(colors[i]);
                }
                strcat(new_file_path, ss.str().c_str());
                strcat(new_file_path, "_000.bmp");

                int count = 0;
                while (fileExists(new_file_path))
                {
                    count++;
                    std::stringstream ss;
                    ss << std::setfill('0') << std::setw(3) << count;
                    char *new_num = new char[4];
                    strcpy(new_num, ss.str().c_str());
                    memcpy(new_file_path + strlen(new_path) + 10, new_num, 3);
                    delete[] new_num;
                }

                writeBMP(new_file_path, width, height, data);
                printf("%s\n", new_file_path);

                // delete[] name;
                delete[] colors;
                delete[] new_file_path;
                delete[] file_path;
                delete[] data;
            }
        }
        closedir(dir);
    }
}

void imageprocess::clearDir(char* path)
{
    DIR *dir;
    struct dirent *ent;
    
    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_name[0] != '.')
            {
                char* file_path = new char[ent->d_namlen + 17 + 1];
                strcpy(file_path, path);
                strcat(file_path, ent->d_name);

                remove(file_path);
            }
        }
    }
}

void imageprocess::averageColor(unsigned char *bmp, int width, int height, unsigned char *color)
{
    int r = 0, g = 0, b = 0;
    for (int i = 0; i < width * height; i++)
    {
        r += int(bmp[i * 3]);
        g += int(bmp[i * 3 + 1]);
        b += int(bmp[i * 3 + 2]);
    }
    color[0] = int(r / (width * height));
    color[1] = int(g / (width * height));
    color[2] = int(b / (width * height));
}

unsigned char *imageprocess::crop(unsigned char *bmp, int width, int height, int new_width, int new_height)
{
    int diff_x = width - new_width;
    int diff_y = height - new_height;
    int offset_x = diff_x / 2;
    int offset_y = diff_y / 2;

    unsigned char *new_data = new unsigned char[width * height * 3];

    // Not accpeting smaller image
    if (diff_x >= 0 && diff_y >= 0)
    {
        for (int i = 0; i < height - diff_y; i++)
        {
            memcpy(new_data + (i * new_width * 3), bmp + (((i + offset_y) * width + offset_x) * 3), 3 * new_width);
        }
    }
    else
    {
        delete[] new_data;
        return NULL;
    }

    return new_data;
}

unsigned char *imageprocess::resize(unsigned char *bmp, int width, int height, int scale, bool scaleup)
{
    if (scaleup)
        return NULL;

    int new_width = width * (scaleup ? scale : (1.0 / scale));
    int new_height = height * (scaleup ? scale : (1.0 / scale));

    unsigned char *new_data = new unsigned char[new_width * height * 3];

    int step = scaleup ? 1 : scale;
    for (int i = 0; i < height; i++)
    {
        int n = 0;
        double x = (double(n) - 0.5) * (scaleup ? scale : (1.0 / scale)) - 0.5;
        int j = scaleup ? -1 : 0;
        int end = scaleup ? width : (width - step);

        for (j; j < end; j += step)
        {
            int p0 = j - step, p1 = j, p2 = j + step, p3 = j + step * 2;
            int c0[3], c1[3], c2[3], c3[3];
            int a[3], b[3], c[3], d[3];

            double f0[3], f_0[3], f1[3], f_1[3];

            if (p0 < 0)         p0 = 0;
            if (p1 < 0)         p1 = 0;
            if (p2 >= width)    p2 = width - 1;
            if (p3 >= width)    p3 = width - 1;

            for (int k = 0; k < 3; k++)
            {
                c0[k] = bmp[(i * width + p0) * 3 + k];
                f0[k] = c1[k] = bmp[(i * width + p1) * 3 + k];
                f1[k] = c2[k] = bmp[(i * width + p2) * 3 + k];
                c3[k] = bmp[(i * width + p3) * 3 + k];
                f_0[k] = double(c2[k] - c0[k]) / (scale * 2);
                f_1[k] = double(c3[k] - c1[k]) / (scale * 2);

                a[k] = (2 * f0[k]) - (2 * f1[k]) + f_0[k] + f_1[k];
                b[k] = 0 - (3 * f0[k]) + (3 * f1[k]) - (2 * f_0[k]) - f_1[k];
                c[k] = f_0[k];
                d[k] = f0[k];
            }

            while (x < j + step)
            {
                for (int k = 0; k < 3; k++)
                {
                    int color = int((a[k] * pow(x, 3)) + (b[k] * pow(x, 2)) + (c[k] * x) + d[k]);
                    if (color > 255)    color = 255;
                    if (color < 0)      color = 0;

                    new_data[(i * new_width + n) * 3 + k] = (unsigned char)(color);
                }

                n++;
                x = (double(n) - 0.5) * (scaleup ? (1.0 / scale) : scale);
                x -= 0.5;
            }

            double temp = 0.0;
            x = std::modf(x, &temp);
        }
    }
    
    unsigned char *new_new_data = new unsigned char[new_width * new_height * 3];
    
    for (int i = 0; i < new_width; i++)
    {
        int n = 0;
        double x = (double(n) - 0.5) * (scaleup ? scale : (1.0 / scale)) - 0.5;
        int j = scaleup ? -1 : 0;
        int end = scaleup ? height : (height - step);

        for (j; j < end; j += step)
        {
            int p0 = j - step, p1 = j, p2 = j + step, p3 = j + step * 2;
            int c0[3], c1[3], c2[3], c3[3];
            int a[3], b[3], c[3], d[3];

            double f0[3], f_0[3], f1[3], f_1[3];

            if (p0 < 0)         p0 = 0;
            if (p1 < 0)         p1 = 0;
            if (p2 >= height)   p2 = height - 1;
            if (p3 >= height)   p3 = height - 1;

            for (int k = 0; k < 3; k++)
            {
                c0[k] = new_data[(p0 * new_width + i) * 3 + k];
                f0[k] = c1[k] = new_data[(p1 * new_width + i) * 3 + k];
                f1[k] = c2[k] = new_data[(p2 * new_width + i) * 3 + k];
                c3[k] = new_data[(p2 * new_width + i) * 3 + k];
                f_0[k] = double(c2[k] - c0[k]) / (scale * 2);
                f_1[k] = double(c3[k] - c1[k]) / (scale * 2);

                a[k] = (2 * f0[k]) - (2 * f1[k]) + f_0[k] + f_1[k];
                b[k] = 0 - (3 * f0[k]) + (3 * f1[k]) - (2 * f_0[k]) - f_1[k];
                c[k] = f_0[k];
                d[k] = f0[k];
            }

            while (x < j + step)
            {
                for (int k = 0; k < 3; k++)
                {
                    int color = int((a[k] * pow(x, 3)) + (b[k] * pow(x, 2)) + (c[k] * x) + d[k]);
                    if (color > 255)    color = 255;
                    if (color < 0)      color = 0;

                    new_new_data[(n * new_width + i) * 3 + k] = (unsigned char)(color);
                }

                n++;
                x = (double(n) - 0.5) * (scaleup ? (1.0 / scale) : scale);
                x -= 0.5;
            }

            double temp = 0.0;
            x = std::modf(x, &temp);
        }
    }

    delete[] new_data;

    return new_new_data;
}

bool imageprocess::fileExists(char *path)
{
    if (FILE *file = fopen(path, "r"))
    {
        fclose(file);
        return true;
    }
    else
    {
        return false;
    }
}

ThreeDTree* imageprocess::getProcessedFiles()
{
    DIR *dir;
    struct dirent *ent;
    int count = 0;

    char* path = "images/processed/";
    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_name[0] != '.')
            {
                count++;
            }
        }
        closedir(dir);
    }
    
    char** files = new char*[count];
    int color_count = count;
    int i = 0;

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_name[0] != '.')
            {
                files[i] = new char[ent->d_namlen + 1];
                strcpy(files[i], ent->d_name);

                if (i > 0 && sameColor(files[i], files[i - 1]))
                    color_count--;

                i++;
            }
        }
        closedir(dir);
    }

    ThreeDTree::Color** colors = new ThreeDTree::Color*[color_count];
    int j = 0;

    for (int i = 0; i < count; i++)
    {
        if (i != 0)
        {
            if (sameColor(files[i], files[i - 1]))
            {
                colors[j - 1]->add();
                continue;
            }
        }

        int c[3];
        for (int k = 0; k < 3; k++)
        {
            char str[3];
            memcpy(str, files[i] + k * 3, 3);
            c[k] = std::stoi(str);
        }
        ThreeDTree::Color* color = new ThreeDTree::Color(c[0], c[1], c[2]);
        color->add();
        colors[j] = color;
        j++;
    }

    ThreeDTree* root = new ThreeDTree(colors, color_count);
    delete[] colors;

    return root;
}

bool imageprocess::sameColor(char* file1, char* file2)
{
    for (int i = 0; i < 9; i++)
    {
        if (file1[i] != file2[i])
            return false;
    }

    return true;
}

char* imageprocess::getFileName(char* path, ThreeDTree::Color* color)
{
    char* file_path = new char[strlen(path) + 17 + 1];
    strcpy(file_path, path);
    
    int r = rand() % color->getCount();

    std::stringstream ss;
    ss << std::setfill('0')   << std::setw(3) << int(color->r())
                            << std::setw(3) << int(color->g())
                            << std::setw(3) << int(color->b())
                            << "_" << std::setw(3) << r << ".bmp";

    strcat(file_path, ss.str().c_str());

    if (!fileExists(file_path))
    {
        delete[] file_path;
        return NULL;
    }

    return file_path;
}