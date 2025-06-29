#define W 400
#define H 150

void print_display(int display[H][W]);
void clear_display(int display[H][W], int color);
void draw_line(int display[H][W], int x1, int y1, int x2, int y2, int color);
void draw_text(int display[H][W], int x, int y, const char *text, int s,
               int color);
void draw_bar(int display[H][W], int x, int y, double p, int color);