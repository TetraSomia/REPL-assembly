#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include "repl.h"

static char *_allocate_string() {
  char *line;

  puts("Please enter the string value:");
  line = readline("> ");
  if (line == NULL)
    p_warning("Aborting string allocation\n");
  return line;
}

static int _print_arrays() {
  if (!context.arrays)
    return p_warning("No arrays allocated yet.\n");
  puts("Listing arrays:");
  for (int i = 0; context.arrays[i].name; ++i) {
    if (context.arrays[i].is_str)
      printf("%s: %p: '%s'\n", context.arrays[i].name,
	     context.arrays[i].array, context.arrays[i].array);
    else {
      printf("%s: %p: size=%ld: ", context.arrays[i].name,
	     context.arrays[i].array, context.arrays[i].size);
      for (size_t j = 0; j < context.arrays[i].size; ++j)
	printf("%02x ", context.arrays[i].array[j]);
      printf("\n");
    }
  }
  return 0;
}

static bool _array_already_exists(const char *name) {
  if (!context.arrays)
    return false;
  for (int i = 0; context.arrays[i].name; ++i)
    if (strcmp(context.arrays[i].name, name) == 0)
      return true;
  return false;
}

int cmd_array(int ac, char* const *av) {
  s_array array;
  int nbr_a;

  if (ac == 0)
    return _print_arrays();

  if (!str_is_symbol_name(av[0]))
    return p_error("Array's name must follow symbol naming rules.\n");
  if (_array_already_exists(av[0]))
    return p_error("Another array is already named '%s'.\n", av[0]);

  if (ac == 1) {
    array.is_str = true;
    if ((array.array = (uint8_t*)_allocate_string()) == NULL)
      return 1;
    array.size = strlen((char*)array.array);
  } else {
    if (sscanf(av[1], "%ld", &array.size) != 1)
      return p_error("Cannot parse second argument '%s' into a size.\n", av[1]);
    array.array = xmalloc(array.size);
    memset(array.array, 0, array.size);
    array.is_str = false;
  }
  array.name = xstrdup(av[0]);

  for (nbr_a = 0; context.arrays && context.arrays[nbr_a].name; ++nbr_a);
  context.arrays = xrealloc(context.arrays, sizeof(s_array) * (nbr_a + 2));
  memset(&context.arrays[nbr_a + 1], 0, sizeof(s_array));
  memcpy(&context.arrays[nbr_a], &array, sizeof(s_array));
  return 0;
}
