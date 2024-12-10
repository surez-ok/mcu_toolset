#ifndef __MEM_FPOPS_H__
#define __MEM_FPOPS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

typedef struct {

  size_t bsize;           /* buffer size */
  unsigned char *buffer;  /* data trasfer buffer */
  size_t curpos;          /* current pos */

} MEMFILE;

#ifdef UNDEFFILE

#undef FILE
#define FILE MEMFILE

#undef stdin
#define stdin  ((MEMFILE*)0)

#undef stdout
#define stdout ((MEMFILE*)1)

#undef stderr
#define stderr ((MEMFILE*)2)

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#undef fopen
#define fopen   mem_fopen

#undef fclose
#define fclose  mem_fclose

#undef fread
#define fread   mem_fread

#undef fwrite
#define fwrite  mem_fwrite

#undef fgetc
#define fgetc    mem_fgetc

#undef fgets
#define fgets    mem_fgets

#undef fputc
#define fputc    mem_fputc

#undef fputs
#define fputs    mem_fputs

#undef fseek
#define fseek    mem_fseek

#undef ftell
#define ftell    mem_ftell

#undef rewind
#define rewind    mem_rewind

#undef fprintf
#define fprintf mem_fprintf

#undef fscanf
#define fscanf mem_fscanf

#undef fflush
#define fflush  mem_fflush

#undef ferror
#define ferror  mem_ferror

#endif /* #ifdef UNDEFFILE */

#define EXTERN(type)         extern type

EXTERN(MEMFILE*)  mem_fopen(char *buffer, size_t len);
EXTERN(int)       mem_fclose(MEMFILE *stream);
EXTERN(size_t)    mem_fread(void *buffer, size_t size, size_t count, MEMFILE *fp);
EXTERN(size_t)    mem_fwrite(const void *buffer, size_t size, size_t count, MEMFILE *fp);
EXTERN(int)       mem_fgetc(MEMFILE *stream);
EXTERN(char *)    mem_fgets(char *str, int num, MEMFILE *stream);
EXTERN(int)       mem_fputc(int c, MEMFILE *stream);
EXTERN(int)       mem_fputs(const char *s, MEMFILE *stream);
EXTERN(int)       mem_fseek(MEMFILE* stream, long int offset, int whence);
EXTERN(long int)  mem_ftell(MEMFILE* stream);
EXTERN(void)      mem_rewind(MEMFILE* stream);
EXTERN(int)       mem_feof(MEMFILE* stream);
EXTERN(int)       mem_fprintf(MEMFILE *stream, const char *format , ...);
EXTERN(int)       mem_fscanf(MEMFILE *stream, const char *format , ...);
EXTERN(int)       mem_fflush(MEMFILE *stream);
EXTERN(int)       mem_ferror(MEMFILE *stream);


#endif