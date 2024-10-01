/*
 * gpt.h -- GUID partition table
 */


#ifndef _GPT_H_
#define _GPT_H_


#define GPT_UUID_LEN	37
#define GPT_NAME_LEN	37

#define GPT_NULL_UUID	"00000000-0000-0000-0000-000000000000"


typedef struct {
  char type[GPT_UUID_LEN];
  char uniq[GPT_UUID_LEN];
  unsigned int start;
  unsigned int end;
  unsigned int attr;
  char name[GPT_NAME_LEN];
} GptEntry;


void gptRead(FILE *disk, unsigned int diskSize);
void gptWrite(FILE *disk);

void gptGetEntry(int partNumber, GptEntry *entry);
void gptSetEntry(int partNumber, GptEntry *entry);

#endif /* _GPT_H_ */
