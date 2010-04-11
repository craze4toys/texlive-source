#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kpathsea/config.h>
#include <ptexenc/ptexenc.h>
#include "mendex.h"

#include "exvar.h"

#ifdef KPATHSEA
#include "kp.h"
extern KpathseaSupportInfo kp_ist;
#endif

FILE *fp;

void convline(char *buff1, int start, char *buff2);
int scompare(char *buff1, char *buff2);
int getparam(char *buff, char *paraname, char *param);
int getparachar(char *buff, char *paraname, char *param);
size_t sstrlen(const char *buff);
int sstrcmp(const char *s1, const char *s2);
int sstrncmp(const char *s1, const char *s2, size_t len);

/*   read style file   */
void styread(char *filename)
{
	int i,cc;
	char buff[4096];

#ifdef KPATHSEA
	filename = KP_find_file(&kp_ist,filename);
#endif
	fp=nkf_open(filename,"r");
	if (fp==NULL) {
		fprintf(stderr,"%s does not exist.\n",filename);
		exit(0);
	}

	for (i=0;;i++) {
		if (fgets(buff,4095,fp)==NULL) break;
		if (getparam(buff,"keyword",keyword)) continue;
		if (getparachar(buff,"arg_open",&arg_open)) continue;
		if (getparachar(buff,"arg_close",&arg_close)) continue;
		if (getparachar(buff,"range_open",&range_open)) continue;
		if (getparachar(buff,"range_close",&range_close)) continue;
		if (getparachar(buff,"level",&level)) continue;
		if (getparachar(buff,"actual",&actual)) continue;
		if (getparachar(buff,"encap",&encap)) continue;
		if (getparachar(buff,"quote",&quote)) continue;
		if (getparachar(buff,"escape",&escape)) continue;
		if (getparam(buff,"preamble",preamble)) continue;
		if (getparam(buff,"postamble",postamble)) continue;
		if (getparam(buff,"group_skip",group_skip)) continue;
		if (getparam(buff,"lethead_prefix",lethead_prefix)) continue;
		if (getparam(buff,"heading_prefix",lethead_prefix)) continue;
		if (getparam(buff,"lethead_suffix",lethead_suffix)) continue;
		if (getparam(buff,"heading_suffix",lethead_suffix)) continue;
		if (getparam(buff,"symhead_positive",symhead_positive)) continue;
		if (getparam(buff,"symhead_negative",symhead_negative)) continue;
		if (getparam(buff,"numhead_positive",numhead_positive)) continue;
		if (getparam(buff,"numhead_negative",numhead_negative)) continue;
		cc=scompare(buff,"lethead_flag");
		if (cc!= -1) {
			lethead_flag=atoi(&buff[cc]);
			continue;
		}
		cc=scompare(buff,"heading_flag");
		if (cc!= -1) {
			lethead_flag=atoi(&buff[cc]);
			continue;
		}
		cc=scompare(buff,"priority");
		if (cc!= -1) {
			priority=atoi(&buff[cc]);
			continue;
		}
		if (getparam(buff,"item_0",item_0)) continue;
		if (getparam(buff,"item_1",item_1)) continue;
		if (getparam(buff,"item_2",item_2)) continue;
		if (getparam(buff,"item_2",item_2)) continue;
		if (getparam(buff,"item_01",item_01)) continue;
		if (getparam(buff,"item_x1",item_x1)) continue;
		if (getparam(buff,"item_12",item_12)) continue;
		if (getparam(buff,"item_x2",item_x2)) continue;
		if (getparam(buff,"delim_0",delim_0)) continue;
		if (getparam(buff,"delim_1",delim_1)) continue;
		if (getparam(buff,"delim_2",delim_2)) continue;
		if (getparam(buff,"delim_n",delim_n)) continue;
		if (getparam(buff,"delim_r",delim_r)) continue;
		if (getparam(buff,"delim_t",delim_t)) continue;
		if (getparam(buff,"suffix_2p",suffix_2p)) continue;
		if (getparam(buff,"suffix_3p",suffix_3p)) continue;
		if (getparam(buff,"suffix_mp",suffix_mp)) continue;
		if (getparam(buff,"encap_prefix",encap_prefix)) continue;
		if (getparam(buff,"encap_infix",encap_infix)) continue;
		if (getparam(buff,"encap_suffix",encap_suffix)) continue;
		cc=scompare(buff,"line_max");
		if (cc!= -1) {
			line_max=atoi(&buff[cc]);
			continue;
		}
		if (getparam(buff,"indent_space",indent_space)) continue;
		cc=scompare(buff,"indent_length");
		if (cc!= -1) {
			indent_length=atoi(&buff[cc]);
			continue;
		}
		if (getparam(buff,"symbol",symbol)) continue;
		cc=scompare(buff,"symbol_flag");
		if (cc!= -1) {
			symbol_flag=atoi(&buff[cc]);
			continue;
		}
		cc=scompare(buff,"letter_head");
		if (cc!= -1) {
			letter_head=atoi(&buff[cc]);
			continue;
		}
		if (getparam(buff,"atama",atama)) continue;
		if (getparam(buff,"page_compositor",page_compositor)) continue;
		if (getparam(buff,"page_precedence",page_precedence)) continue;
		if (getparam(buff,"character_order",character_order)) continue;
	}
	nkf_close(fp);
}

/*   analize string parameter of style file   */
void convline(char *buff1, int start, char *buff2)
{
	int i,j,cc;

	for (i=start,j=cc=0;;i++) {
		if (buff1[i]=='\"') {
			if (cc==0) {
				cc=1;
			}
			else {
				buff2[j]='\0';
				break;
			}
		}
		else if ((buff1[i]=='\n')||(buff1[i]=='\0')) {

			if (cc==1) {
				buff2[j++]='\n';
			}
			if (fgets(buff1,4095,fp)==NULL) {
				buff2[j]='\0';
				break;
			}
			i= -1;
		}
		else if (cc==1) {
			if ((unsigned char)buff1[i]>0x80) {
				 buff2[j++]=buff1[i++];
				 buff2[j]=buff1[i];
			}
			else if (buff1[i]=='\\') {
				i++;
				if (buff1[i]=='\\') buff2[j]='\\';
				else if (buff1[i]=='n') buff2[j]='\n';
				else if (buff1[i]=='t') buff2[j]='\t';
				else if (buff1[i]=='r') buff2[j]='\r';
				else if (buff1[i]=='\"') buff2[j]='\"';
			}
			else buff2[j]=buff1[i];
			j++;
		}
	}
}

/*   compare strings   */
int scompare(char *buff1, char *buff2)
{
	int i;

	if (sstrlen(buff1) == sstrlen(buff2)) {
		if (sstrcmp(buff1,buff2) == 0) {
			if (fgets(buff1,4095,fp)==NULL) return -1;
			return 0;
		}
		return -1;
	}
	for (i=0;i<sstrlen(buff1)-sstrlen(buff2);i++) {
		if (buff1[i]==' ' || buff1[i]=='\t') {
			continue;
		}
		else if ((sstrncmp(&buff1[i],buff2,sstrlen(buff2))==0)
			&&((buff1[i+strlen(buff2)]==' ')||(buff1[i+strlen(buff2)]=='\t')||(buff1[i+strlen(buff2)]=='\n'))) {
			return i+strlen(buff2);
		}
		else return -1;
	}
	return -1;
}

/*   get string of style patameter   */
int getparam(char *buff, char *paraname, char *param)
{
	int cc;

	cc=scompare(buff,paraname);
	if (cc!=-1) {
		convline(buff,cc,param);
		return 1;
	}
	return 0;
}

/*   get character of style parameter   */
int getparachar(char *buff, char *paraname, char *param)
{
	int j,cc;

	cc=scompare(buff,paraname);
	if (cc!=-1) {
		for (j=cc;j<strlen(buff);j++) {
			if (buff[j]=='\'') {
				(*param)=buff[j+1];
				break;
			}
			else if (buff[j]=='\n') {
				if (fgets(buff,4095,fp)==NULL) {
					break;
				}
				else j= -1;
			}
		}
		return 1;
	}
	return 0;
}

size_t sstrlen(const char *s)
{
	if (s == NULL) return 0;
	return strlen(s);
}

int sstrcmp(const char *s1, const char *s2)
{
	if (s1 == NULL) return -1;
	if (s2 == NULL) return 1;
	return strcmp(s1, s2);
}

int sstrncmp(const char *s1, const char *s2, size_t len)
{
	if (s1 == NULL) return -1;
	if (s2 == NULL) return 1;
	return strncmp(s1, s2, len);
}
