#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "id3v2lib.h"

#define MAX_FILE_PATH 4096

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MAX3(a, b, c) (MAX(MAX(a, b), c))
#define MAX4(a, b, c, d) (MAX(MAX3(a, b, c), d))
#define MAX5(a, b, c, d, e) (MAX(MAX4(a, b, c, d), e))
#define MAX6(a, b, c, d, e, f) (MAX(MAX5(a, b, c, d, e), f))
#define MAX7(a, b, c, d, e, f, g) (MAX(MAX6(a, b, c, d, e, f), g))
#define MAX8(a, b, c, d, e, f, g, h) (MAX(MAX7(a, b, c, d, e, f, g), h))
#define MAX9(a, b, c, d, e, f, g, h, i) (MAX(MAX8(a, b, c, d, e, f, g, h), i))
#define MAX10(a, b, c, d, e, f, g, h, i, j) \
                (MAX(MAX9(a, b, c, d, e, f, g, h, i), j))
#define MAX11(a, b, c, d, e, f, g, h, i, j, k) \
                (MAX(MAX10(a, b, c, d, e, f, g, h, i, j), k))

char * str_copy(char *buf, 
                int buf_size,
                char *data, 
                int data_size)
{
    memset(buf, 0, buf_size);
    strncpy(buf, data, data_size);
    return buf;
}

void rewrite_tag(const char *filename)
{
    char *buf;
    int buf_len ;
    ID3v2_tag *tag = load_tag(filename); // Load the full tag from the file

    if(tag == NULL){
        tag = new_tag();
        puts("NO TAGS!");
        return;
    }

    /* Get frames */
    ID3v2_frame *title_frame = tag_get_title(tag);
    ID3v2_frame *artist_frame = tag_get_artist(tag);
    ID3v2_frame *album_frame = tag_get_album(tag);
    // ID3v2_frame *album_cover_frame = tag_get_album_cover(tag);
    // ID3v2_frame *album_artist_frame = tag_get_album_artist(tag);
    // ID3v2_frame *genre_frame = tag_get_genre(tag);
    // ID3v2_frame *track_frame = tag_get_track(tag);
    // ID3v2_frame *year_frame = tag_get_year(tag);
    // ID3v2_frame *comment_frame = tag_get_comment(tag);
    // ID3v2_frame *disc_number_frame = tag_get_disc_number(tag);
    // ID3v2_frame *composer_frame = tag_get_composer(tag);
    
    ID3v2_frame_text_content *title_content = 
            parse_text_frame_content(title_frame);
    ID3v2_frame_text_content *artist_content = 
            parse_text_frame_content(artist_frame); 
    ID3v2_frame_text_content *album_content = 
            parse_text_frame_content(album_frame);
    // ID3v2_frame_text_content *album_cover_content = 
    //         parse_text_frame_content(album_cover_frame);
    // ID3v2_frame_text_content *album_artist_content = 
    //         parse_text_frame_content(album_artist_frame);
    // ID3v2_frame_text_content *genre_content = 
    //         parse_text_frame_content(genre_frame);
    // ID3v2_frame_text_content *track_content = 
    //         parse_text_frame_content(track_frame);
    // ID3v2_frame_text_content *year_content = 
    //         parse_text_frame_content(year_frame);
    // ID3v2_frame_text_content *comment_content = 
    //         parse_text_frame_content(comment_frame);
    // ID3v2_frame_text_content *disc_number_content = 
    //         parse_text_frame_content(disc_number_frame);
    // ID3v2_frame_text_content *composer_content = 
    //         parse_text_frame_content(composer_frame);
    
    /* Alloc buffer */
    // buf_len = MAX11(title_content->size, 
    //                 artist_content->size, 
    //                 album_content->size,
    //                 album_cover_content->size,
    //                 album_artist_content->size,
    //                 genre_content->size,
    //                 track_content->size,
    //                 year_content->size,
    //                 comment_content->size,
    //                 disc_number_content->size,
    //                 composer_content->size);
    buf_len = MAX3(title_content->size, 
                    artist_content->size, 
                    album_content->size) + 1;
    buf = malloc(buf_len);

    /* Set frames */
    str_copy(buf, buf_len, title_content->data, title_content->size);
    tag_set_title(buf, 0, tag);

    str_copy(buf, buf_len, artist_content->data, artist_content->size);
    tag_set_artist(buf, 0, tag);

    str_copy(buf, buf_len, album_content->data, album_content->size);
    tag_set_album(buf, 0, tag);

    // str_copy(buf, buf_len, title_content->data, title_content->size);
    // str_copy(buf, buf_len, artist_content->data, artist_content->size);
    // str_copy(buf, buf_len, album_content->data, album_content->size);

    // printf(" TITLE: %s\n", buf);
    // printf("ARTIST: %s\n", buf);
    // printf(" ALBUM: %s\n", buf);
    
    /* Set tag */
    set_tag(filename, tag);

    /* Free */
    free(buf);
    free_tag(tag);
}

typedef struct {
    char data[MAX_FILE_PATH];
    int size;
    int overflow;
} InputBuffer;

void InputBufferCreate(InputBuffer *buf)
{
    buf->size = 0;
    buf->overflow = 0;
}

int InputBufferRead(InputBuffer *buf)
{
    int c;

    while ((c = getchar()) != EOF) {
        if (c != '\r') {
            if (c != '\n') {
                if (buf->size < MAX_FILE_PATH) {
                    buf->data[buf->size++] = c;
                } else {
                    buf->overflow = 1;
                }
            } else {
                buf->data[buf->size] = '\0';
                return 1;
            }
        }
    }

    return 0;
}

int InputBufferReadEnd(InputBuffer *buf)
{
    if (buf->size) {
        buf->data[buf->size] = '\0';
        return 1;
    }

    return 0;
}

void InputBufferReset(InputBuffer *buf)
{
    buf->size = 0;
    buf->overflow = 0;
}

void InputBufferProcess(InputBuffer *buf,
                        void (*process_fn)(const char *),
                        void (*error_fn)(const char *))
{
    if (buf->overflow) {
        error_fn(buf->data);
    } else {
        process_fn(buf->data);
    }
    InputBufferReset(buf);
}

void process_filename(const char *filename)
{
    printf("%s\n", filename);
    rewrite_tag(filename);
}

void process_error_filename(const char *filename)
{
    fprintf(stderr, "Error: filepath too long! (%s)\n", filename);
}

void core()
{
    InputBuffer buf;

    InputBufferCreate(&buf);

    while (InputBufferRead(&buf)) {
        InputBufferProcess(&buf, process_filename, process_error_filename);
    }

    if (InputBufferReadEnd(&buf)) {
        InputBufferProcess(&buf, process_filename, process_error_filename);
    }
}

int main(int argc, 
         char **argv) 
{
    core();
    return 0;
}
