#include <pcre.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <Ruby/ruby/ruby.h>

#define DEBUG false

typedef struct
{
  int length;
  const char **matchs;
} Matchs;

pcre *compileRegex(const char *pattern,
                   pcre_extra *pcreExtra)
{
  pcre *regexCompiled;
  int pcreErrorOffset;
  const char *pcreErrorStr;

  regexCompiled = pcre_compile(
      pattern,
      0,
      &pcreErrorStr,
      &pcreErrorOffset,
      NULL);

  if (regexCompiled == NULL)
  {
    if (DEBUG)
      printf("ERROR - Could not compile\n");
    return NULL;
  }

  pcreExtra = pcre_study(regexCompiled, 0, &pcreErrorStr);

  if (pcreErrorStr != NULL)
  {
    if (DEBUG)
      printf("ERROR - Could not study\n");
    return NULL;
  }

  return regexCompiled;
}

Matchs *match(
    const char *target,
    pcre *regexCompiled,
    pcre_extra *pcreExtra)
{
  Matchs *m;
  int l;
  int subStrVecLen;
  int *subStrVec;
  const char *psubStrMatchStr;

  m = NULL;
  subStrVecLen = strlen(target) + 1;
  subStrVec = (int *)malloc(sizeof(int) * subStrVecLen);

  int pcreExecRet = pcre_exec(regexCompiled,
                              pcreExtra,
                              target,
                              strlen(target),
                              0,
                              0,
                              subStrVec,
                              subStrVecLen);

  if (pcreExecRet < 0)
  {
    switch (pcreExecRet)
    {
    case PCRE_ERROR_NOMATCH:
      if (DEBUG)
        printf("String did not match the pattern\n");
      break;
    case PCRE_ERROR_NULL:
      if (DEBUG)
        printf("Something was null\n");
      break;
    case PCRE_ERROR_BADOPTION:
      if (DEBUG)
        printf("A bad option was passed\n");
      break;
    case PCRE_ERROR_BADMAGIC:
      if (DEBUG)
        printf("Magic number bad (compiled re corrupt?)\n");
      break;
    case PCRE_ERROR_UNKNOWN_NODE:
      if (DEBUG)
        printf("Something kooky in the compiled re\n");
      break;
    case PCRE_ERROR_NOMEMORY:
      if (DEBUG)
        printf("Ran out of memory\n");
      break;
    default:
      if (DEBUG)
        printf("Unknown error\n");
      break;
    }
  }
  else
  {
    m = (Matchs *)malloc(sizeof(Matchs));
    m->length = pcreExecRet;
    m->matchs = (const char **)malloc(sizeof(char *) * pcreExecRet);

    for (int j = 0; j < pcreExecRet; j++)
    {
      pcre_get_substring(target, subStrVec, pcreExecRet, j, &(psubStrMatchStr));
      m->matchs[j] = (char *)malloc(sizeof(char) * strlen(psubStrMatchStr) + 1);
      m->matchs[j] = psubStrMatchStr;
    }
  }

  free(subStrVec);

  return m;
}

void freeMatchs(Matchs *m)
{
  if (m != NULL)
  {
    free(m->matchs);
    free(m);
  }
}

typedef struct
{
  pcre *pc;
  pcre_extra *pe;
} PCRE_WRAP;

PCRE_WRAP pw;

static void PW_free(void *ptr)
{
  free(ptr);
}

const rb_data_type_t PCRE_data_type = {
    "PCRE",
    {
        NULL,
        PW_free,
        NULL,
    },
    NULL,
    NULL};

static VALUE
PCRE_initialize(VALUE self, VALUE str)
{
  char *ptr = RSTRING_PTR(str);
  pcre_extra *pe = NULL;
  pcre *v_pcre = compileRegex(ptr, pe);
  pw.pc = v_pcre;
  pw.pe = pe;

  return self;
}

static VALUE
PCRE_match(VALUE self, VALUE pattern)
{
  VALUE ret = rb_ary_new();
  char *target = RSTRING_PTR(pattern);
  Matchs *matchs = match(target, pw.pc, pw.pe);

  if (matchs != NULL)
  {
    for (int i = 0; i < matchs->length; i++)
    {
      rb_ary_push(ret, rb_str_new_cstr(matchs->matchs[i]));
    }
  }

  return ret;
}

static VALUE cPCRE;

void Init_pcre()
{
  cPCRE = rb_define_class("PCRE", rb_cObject);
  rb_define_private_method(cPCRE, "initialize", PCRE_initialize, 1);
  rb_define_method(cPCRE, "match", PCRE_match, 1);
}
