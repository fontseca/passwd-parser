#include <stdio.h>
#include <stdlib.h>
#include <iso646.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <errno.h>

/* Checks whether the given path is a directory.  */
static int is_directory(const char *path)
{
  struct stat statbuf;
  return stat(path, &statbuf) != 0 ? 0 : S_ISDIR(statbuf.st_mode);
}

/* Abstracts a user in the `/etc/passwd' UNIX file.  */
typedef struct
{
  /* The name of the user.  */
  char name[0x100];

  /* The password of the user (onlye one character.)  */
  char pass_c;

  /* The UID of the user.  */
  unsigned int uid;

  /* The GID of the user.  */
  unsigned int gid;

  /* Information about the user.  */
  char info[0x500];

  /* The user's home directory.  */
  char home[0x500];

  /* The user's shell.  */
  char shell[0x500];
} user_t;

/* Prints a user to stdout.  */
static void print_parsed_user(user_t *const user)
{
  fprintf(stdout, "{\n\
  Name:      `%s'\n\
  Password:  `%c'\n\
  UID:       `%u'\n\
  GID:       `%u'\n\
  Info:      `%s'\n\
  Home dir:  `%s'\n\
  Shell dir: `%s'\n\
}\n", user->name, user->pass_c, user->uid,
  user->gid, user->info, user->home, user->shell);
}

int32_t main(int32_t argc, char *argv[])
{
  /* Accept only one argument.  */
  if (2 > argc or argc > 2)
  {
    fputs("error: Bad number of argument. Expected only one.\n", stderr);
    exit(EXIT_FAILURE);
  }

  /* Name of the file to read.  */
  char *filename = argv[1];

  /* We do not want directories.  */
  if (is_directory(filename))
  {
    fprintf(stderr, "error: %s: Is a directory.\n", filename);
    exit(EXIT_FAILURE); 
  }

  /* Text stream to read from.  */
  FILE *stream = fopen(filename, "r");

  /* Could not establish connection with the
    stream text stream.  */
  if (not stream)
  {
    fprintf(stderr, "error: %s.\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  /* A line in the stream.  */
  char line[0x400];

  while (fgets(line, sizeof line, stream)
    and not feof(stream) and not ferror(stream))
  {
    /* The next user to be parsed.  */
    user_t user;

    /* Try to parse next user.  */
    if (7 < sscanf(line, "%[0-9a-zA-Z_-]:%c:%u:%u:%[a-zA-Z, ]:%[0-9a-zA-Z_-/ ]:%[0-9a-zA-Z_- / ]",
    user.name, &user.pass_c, &user.uid, &user.gid, user.info, user.home, user.shell))
    {
      fputs("error: Could not parse user.\n", stderr);
      exit(EXIT_FAILURE);
    }

    /* Print user.  */
    print_parsed_user(&user);
  }

  fclose(stream);

  return EXIT_SUCCESS;
}
