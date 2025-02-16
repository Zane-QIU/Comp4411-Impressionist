#ifndef THREEDTREE_H
#define THREEDTREE_H

class ThreeDTree
{
public:    
    class Color
    {
    public:
        Color();
        Color(int r, int g, int b);
        Color(unsigned char* color);
        
        ~Color();

        int r() const;
        int g() const;
        int b() const;
        int getCount() const;

        int dist(Color* another);
        void add();

    private:
        unsigned char* rgb;
        int count;
    };

    enum RGB
    {
        R = 0,
        G,
        B
    };

    ThreeDTree(Color** colors, int size, RGB rgb = R);
    Color* SearchNearest(Color* target);

private:
    ThreeDTree *left, *right;
    Color* color;
    RGB dim;

    int dist(ThreeDTree* another);

    static void sortColor(Color** colors, int size, RGB rgb);
    static void split(Color** colors, int size, int& size_l, int& size_r, Color** &l, Color** &r);
    static void merge(Color** &colors, int size_l, int size_r, Color** l, Color** r, RGB rgb);

    static ThreeDTree* search(ThreeDTree* root, Color* target);
};

#endif