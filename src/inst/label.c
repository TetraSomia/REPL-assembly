#include <stdlib.h>
#include <string.h>
#include "code_unit.h"

void set_label(s_code_instruction *inst, const char *label) {
  if (inst->label && strlen(label) < strlen(inst->label))
    strcpy(inst->label, label);
  else {
    free(inst->label);
    inst->label = xstrdup(label);
  }
}

bool reset_label(s_code_instruction *inst) {
  if (inst->label == NULL)
    return false;
  free(inst->label);
  inst->label = NULL;
  return true;
}
