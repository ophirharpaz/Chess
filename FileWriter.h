/*
 * FileWriter.h
 *
 *  Created on: Jul 29, 2015
 *      Author: linesh
 */

#ifndef FILEWRITER_H_
#define FILEWRITER_H_

#include "DataStructures.h"

int load_file(char * path, PtrConfig c);
int save_file(char * path, PtrConfig c);
char * next_line(char *p);
int compare_line_field(char *p,const char *text);
void load_line(PtrConfig c,char *p,int row);
int write_line(FILE * fp,char * field,char * content, int depth);

#endif /* FILEWRITER_H_ */
