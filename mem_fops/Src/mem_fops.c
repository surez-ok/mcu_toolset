#include "mem_fops.h"

MEMFILE* mem_fopen(char *buffer, size_t len)
{
  MEMFILE *fp = (MEMFILE *)malloc(sizeof(MEMFILE));

  fp->buffer = buffer;
  fp->bsize = len;
  fp->curpos = 0;

  return fp;
}

int mem_fclose(MEMFILE *stream)
{
  if (stream == NULL)
    return 0;

  free(stream);
  stream = NULL;

  return 0;
}

size_t mem_fread(void *buffer, size_t size, size_t count, MEMFILE *fp)
{
  if (fp == NULL || fp->buffer == NULL)
    return 0;

  size_t curpos = fp->curpos;
  unsigned char *pbuff = fp->buffer + curpos;
  size_t len = size * count;
  if (len > fp->bsize - curpos) {
    printf("fread overflow!\r\n");
    return 0;
  }

  for (size_t cnt = 0; cnt < len; cnt++) {
     *(unsigned char *)buffer++ = *pbuff++;
  }
  fp->curpos += len;

  return count;
}

size_t mem_fwrite(const void *buffer, size_t size, size_t count, MEMFILE *fp)
{
  if (fp == NULL || fp->buffer == NULL)
    return 0;

  size_t curpos = fp->curpos;
  unsigned char *pbuff = fp->buffer + curpos;
  size_t len = size * count;
  if (len > fp->bsize - curpos) {
    printf("fwrite overflow!\r\n");
    return 0;
  }

  for (size_t cnt = 0; cnt < len; cnt++) {
     *pbuff++ = *(unsigned char *)buffer++;
  }
  fp->curpos += len;

  return count;
}

int mem_fgetc(MEMFILE *stream)
{
 if (stream == NULL || stream->buffer == NULL)
    return -1;
  size_t curpos = stream->curpos;
  unsigned char *pbuff = stream->buffer + curpos;

  if (curpos >= stream->bsize) {
    printf("fgetc overflow!\r\n");
    return -1;
  }

  int ch = *pbuff;
  stream->curpos += 1;

  return ch;
}

char *mem_fgets(char *str, int num, MEMFILE *stream) {
  if (stream == NULL || stream->buffer == NULL || num <= 0 || str == NULL)
    return NULL;

  char *ptr = str;
  size_t remaining = num - 1;

  while (remaining > 0 && stream->curpos < stream->bsize) {
      unsigned char ch = stream->buffer[stream->curpos++];
      *ptr++ = ch;
      remaining--;

      if (ch == '\n') {
          break;
      }
  }

  *ptr = '\0';

  if (str == ptr) {
      return NULL;
  }

  return str;
}

int mem_fputc(int c, MEMFILE *stream)
{
  if (stream == NULL || stream->buffer == NULL)
    return -1;

  size_t curpos = stream->curpos;
  unsigned char *pbuff = stream->buffer + curpos;
  if (curpos >= stream->bsize) {
    printf("fputc overflow!\r\n");
    return -1;
  }
  *pbuff = c;
  stream->curpos += 1;

  return c;
}

int mem_fputs(const char *s, MEMFILE *stream) {
  if (stream == NULL || stream->buffer == NULL || s == NULL)
    return -1;

  size_t len = strlen(s);
  if (stream->curpos + len > stream->bsize) {
      len = stream->bsize - stream->curpos;
  }
  
  size_t curpos = stream->curpos;
  unsigned char *pbuff = stream->buffer + curpos;
  for (size_t cnt = 0; cnt < len; cnt++) {
     *pbuff++ = *s++;
  }
  stream->curpos += len;

  return 1;
}

int mem_fseek(MEMFILE* stream, long int offset, int whence)
{
  if (stream == NULL || stream->buffer == NULL)
    return -1;

  switch (whence) {
    case SEEK_SET:
      if (offset < 0 || offset > stream->bsize)
        return -1;
      stream->curpos = offset;
      break;
    case SEEK_CUR:
      if (stream->curpos + offset < 0 || (stream->curpos + offset) > stream->bsize) 
        return -1;
      stream->curpos += offset;
      break;
    case SEEK_END:
      if (offset > 0 || -offset > stream->curpos)
        return -1;
      stream->curpos = stream->bsize + offset;
      break;
    default:
      return -1;
  }

  return 0;
}

long int mem_ftell(MEMFILE* stream)
{
  if (stream == NULL || stream->buffer == NULL)
    return -1;
 
  return stream->curpos;
}

void mem_rewind(MEMFILE* stream)
{
  if (stream == NULL || stream->buffer == NULL)
    return;
 
  stream->curpos = 0;
}

int mem_feof(MEMFILE* stream)
{
  if (stream == NULL || stream->buffer == NULL)
    return -1;
 
  size_t curpos = stream->curpos;
  if (curpos >= stream->bsize) {
    return 1;
  }
  
  return 0;
}

int mem_fprintf(MEMFILE *stream, const char *format, ...)
{
  va_list ap;
  int ret = 0;
  
  if (stream == NULL || stream->buffer == NULL)
    return 0;
  size_t curpos = stream->curpos;
  unsigned char *pbuff = stream->buffer + curpos;

  va_start(ap, format);
  ret = vsprintf(pbuff, format, ap);
  va_end(ap);
  stream->curpos += ret;

  return ret;
}

/* Note: need use newlibc */
#define BUFFER_SIZE 256
int mem_fscanf(MEMFILE *stream, const char *format, ...)
{

  va_list ap;
  
  if (stream == NULL || stream->buffer == NULL)
    return 0;

  char buffer[BUFFER_SIZE];
  if (mem_fgets(buffer, BUFFER_SIZE, stream) != NULL) {
    va_start(ap, format);
    vsscanf(buffer, format, ap);
    va_end(ap);
  }
  return 0;
}

int mem_fflush(MEMFILE *stream)
{
  return 0;
}

int mem_ferror(MEMFILE *stream)
{
  return 0;
}
