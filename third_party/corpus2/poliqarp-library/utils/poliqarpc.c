/*
 * This file is part of the Poliqarp suite.
 * 
 * Copyright (C) 2008-2009 by Instytut Podstaw Informatyki Polskiej
 * Akademii Nauk (IPI PAN; Institute of Computer Science, Polish
 * Academy of Sciences; cf. www.ipipan.waw.pl).  All rights reserved.
 * 
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file gpl.txt included in the packaging
 * of this file.  (See http://www.gnu.org/licenses/translations.html for
 * unofficial translations.)
 * 
 * A commercial license is available from IPI PAN (contact
 * Michal.Ciesiolka@ipipan.waw.pl or ipi@ipipan.waw.pl for more
 * information).  Licensees holding a valid commercial license from IPI
 * PAN may use this file in accordance with that license.
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.
 */

#include <poliqarp-config.h>

#include <sakura/poliqarp.h>
#include <sakura/random.h>
#include <stdio.h>
#include <getopt.h>
#include <errno.h>

#define DEFAULT_CONTEXT_WIDTH 5
#define DEFAULT_LIMIT 1000

bool quiet = false;
bool random_sample = false;
char *corpusname = NULL;
char **query_strings = NULL;
bool tags_context = false;
bool tags_match = true;
size_t context = DEFAULT_CONTEXT_WIDTH;
size_t limit = DEFAULT_LIMIT;
char *query_rewrite = "default";

static void display_usage(const char *progname)
{
   printf(
      "Usage: %s [OPTIONS] CORPUS QUERY\n"
      "Execute a Poliqarp query on a corpus and print its results to\n"
      "standard output in the CSV format.\n\n"
      "  -h, --help              display this help and exit\n"
      "  -v, --version           display version information and exit\n"
      "  -q, --quiet             suppress progress messages printed\n"
      "                          to standard error stream\n"
      "  -c, --context=WIDTH     set context width, in segments\n"
      "  -m, --no-tags-match     don't display base forms and tags in matches\n"
      "  -t, --tags-context      display base forms and tags in contexts\n"
      "  -Q, --query-rewrite=QR  set rewriting rules for implicit queries\n"
      "  -L, --limit=N           produce at most N results\n"
      "  -A, --all               produce all results\n"
      "  -R, --random            produce a random sample\n"
      "\n"
      "Report bugs to <" PACKAGE_BUGREPORT ">.\n",
      progname);
   exit(EXIT_SUCCESS);
}

static void display_version()
{
   printf("Poliqarp command-line client, version %d.%d.%d\n", 
      poliqarp_major_version, poliqarp_minor_version,
      poliqarp_revision_number);
   exit(EXIT_SUCCESS);
}

static void cmdline_process(int argc, char *argv[])
{
   const struct option options[] = {
      { "help",          0, NULL, 'h' },
      { "version",       0, NULL, 'v' },
      { "quiet",         0, NULL, 'q' },
      { "context",       1, NULL, 'c' },
      { "no-tags-match", 0, NULL, 'm' },
      { "tags-context",  0, NULL, 't' },
      { "query-rewrite", 1, NULL, 'Q' },
      { "limit"        , 1, NULL, 'L' },
      { "all",           0, NULL, 'A' },
      { "random",        0, NULL, 'R' },
      { 0,               0, 0,    0 }
   };
   int opt;

   while ((opt = getopt_long(argc, argv, "hvqc:mtQ:L:AR", options, NULL)) != -1)
   {
      switch (opt) {
         case 'h': 
            display_usage(*argv);
            break;
         case 'v':
            display_version();
            break;
         case 'q':
            quiet = true;
            break;
         case 'c':
            {
               char *endptr;
               errno = 0;
               unsigned long context_long = strtoul(optarg, &endptr, 10);
               context = context_long;
               if (*optarg == '\0' || *endptr != '\0'
                  || errno != 0 || context != context_long) {
                  fprintf(stderr, "Incorrect context size\n");
                  exit(EXIT_FAILURE);
               }
            }
            break;
         case 'm':
            tags_match = false;
            break;
         case 't':
            tags_context = true;
            break;
         case 'Q':
            query_rewrite = optarg;
            break;
         case 'L':
            {
               char *endptr;
               errno = 0;
               unsigned long limit_long = strtoul(optarg, &endptr, 10);
               limit = limit_long;
               if (*optarg == '\0' || *endptr != '\0'
                  || errno != 0 || limit != limit_long
                  || limit == 0) {
                  fprintf(stderr, "Incorrect match buffer size\n");
                  exit(EXIT_FAILURE);
               }
            }
            break;
         case 'A':
            limit = (size_t) -1;
            break;
         case 'R':
            random_sample = true;
      }
   }
   if (optind == argc)
      display_usage(*argv);
   corpusname = argv[optind++];
   if (optind == argc)
      display_usage(*argv);
   query_strings = argv + optind;
}

static void quote_comma(const char *s)
{
   while (*s) {
      if (*s == '"') 
         putchar('\\');
      putchar(*s++);
   }
}

static void output_range(struct poliqarp_corpus *corpus, size_t from, size_t to, bool display_tags)
{
   size_t i, j;
   putchar('"');
   for (j = from; j < to; j++) {
      struct poliqarp_segment segment;
      struct poliqarp_segment_info info;
      struct poliqarp_interpretation_set set;
      struct poliqarp_interpretation_set_info sinfo;
      struct poliqarp_interpretation interp;
      struct poliqarp_interpretation_info iinfo;
      
      poliqarp_get_segment(&segment, corpus, j);
      poliqarp_get_segment_info(&segment, &info);
      poliqarp_get_disambiguated_interpretations(&segment, &set);
      poliqarp_get_interpretation_set_info(&set, &sinfo);
      if (info.space_before)
         putchar(' ');
      quote_comma(info.text);
      if (!display_tags)
         continue;
      for (i = 0; i < sinfo.size; i++) {
         poliqarp_get_interpretation(&set, &interp, i);
         poliqarp_get_interpretation_info(&interp, &iinfo);
         fputs(" [", stdout);
         quote_comma(iinfo.base);
         putchar(':');
         quote_comma(iinfo.tag);
         putchar(']');
      }
   }
   putchar('"');
}

static struct poliqarp_corpus corpus;
static struct poliqarp_match_buffer buffer;
static size_t result_counter = 0;

void async_notify_new_results(void *session)
{
   size_t i;
   struct poliqarp_match_buffer_info info;
   poliqarp_get_match_buffer_info(&buffer, &info);
   for (i = 0; i < info.used && result_counter < limit; i++) {
      struct poliqarp_match match;
      struct poliqarp_match document_range;
      size_t lc, rc;
      poliqarp_get_match(&buffer, &match, i);
      poliqarp_get_match_for_document(&corpus, match.document, &document_range);
      lc = match.start < document_range.start + context
         ? document_range.start
         : match.start - context;
      rc = match.end + context > document_range.end
         ? document_range.end
         : match.end + context;
      output_range(&corpus, lc, match.start, tags_context); printf(",");
      if (match.start < match.focus) {
         output_range(&corpus, match.start, match.focus, tags_match); printf(",");
      }
      output_range(&corpus, match.focus, match.end, tags_match); printf(",");
      output_range(&corpus, match.end, rc, tags_context); puts("");
      result_counter++;
   }
   if (result_counter < limit)
      poliqarp_forget(&buffer);
   return;
}

int main(int argc, char *argv[])
{
   struct poliqarp_query query;
   progress_t progress;
   struct poliqarp_error error = poliqarp_error_none;
   int status = EXIT_SUCCESS;

   cmdline_process(argc, argv);
   if (poliqarp_create("" /* use the system locale */, &error) != 0) {
      fprintf(stderr, "%s\n", poliqarp_error_message_get(&error));
      poliqarp_error_message_set(&error, NULL);
      return EXIT_FAILURE;
   }
   progress_init(&progress);

   fprintf(stderr, "Opening corpus...\n");
   if (poliqarp_open_corpus(&corpus, corpusname, &progress, &error) == -1) {
      fprintf(stderr, "%s.\n", poliqarp_error_message_get(&error));
      return EXIT_FAILURE;
   }
   poliqarp_create_match_buffer(&buffer, random_sample ? limit : 0x100);

   struct poliqarp_random_state random_state;
   poliqarp_srandom_time(&random_state);
   while (*query_strings != NULL) {
      fprintf(stderr, "Compiling query...\n");
      if (poliqarp_create_query(&query, *query_strings, &corpus, 0,
         query_rewrite, random_sample ? &random_state : NULL, &error) == -1)
      {
         fprintf(stderr, "Unable to compile query, reason: %s.\n",
            poliqarp_error_message_get(&error));
         status = EXIT_FAILURE;
         goto query_failed;
      }
      fprintf(stderr, "Executing query...\n");
      poliqarp_produce(&buffer, limit, &query, &progress, NULL,
         random_sample ? (size_t) -1 : 0x100, -1);
      async_notify_new_results(NULL);
      fprintf(stderr, "Query successfully executed, %" PRIuSIZE " results found.\n", result_counter);
      poliqarp_destroy_query(&query);
      query_strings++;
      if (*query_strings != NULL)
         printf("\n");
      poliqarp_forget(&buffer);
   }
query_failed:
   poliqarp_error_message_set(&error, NULL);
   poliqarp_destroy_match_buffer(&buffer);
   poliqarp_close_corpus(&corpus);
   poliqarp_destroy();
   return status;
}
