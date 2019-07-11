#include <stdio.h>
#include "getters.h"
#include "context_switch.h"

static uint64_t *_deref_reg(e_register reg, int nbr_deref) {
  uint64_t *addrs[nbr_deref];
  uint64_t *addr;

  if (nbr_deref == 0)
    return 0;
  addr = (uint64_t*)get_reg(reg);
  set_exec_sighandlers();
  for (int i = 0; i < nbr_deref; ++i) {
    addrs[i] = addr;
    addr = (uint64_t*)*addr;
  }
  reset_exec_sighandlers();
  printf("Dereferencing: %s -> ", reg_names[reg]);
  for (int i = 0; i < nbr_deref; ++i) {
    printf("%p", addrs[i]);
    if (i + 1 < nbr_deref)
      printf(" -> ");
  }
  printf("\n");
  return addrs[nbr_deref - 1];
}

static uint64_t *_deref_addr(uint64_t *addr, int nbr_deref) {
  uint64_t *addrs[nbr_deref];

  set_exec_sighandlers();
  for (int i = 0; i < nbr_deref; ++i) {
    addrs[i] = addr;
    addr = (uint64_t*)*addr;
  }
  reset_exec_sighandlers();
  if (nbr_deref < 2)
    return 0;
  printf("Dereferencing: ");
  for (int i = 0; i < nbr_deref; ++i) {
    printf("%p", addrs[i]);
    if (i + 1 < nbr_deref)
      printf(" -> ");
  }
  printf("\n");
  return addrs[nbr_deref - 1];
}

void do_dereferencing(u_parsed_val *v, int nbr_deref) {
  uint64_t *addr = NULL;

  if (v->type == PARSED_REG)
    addr = _deref_reg(v->reg, nbr_deref);
  else
    addr = _deref_addr((uint64_t*)v->addr, nbr_deref + 1);
  if (addr) {
    v->type = PARSED_ADDR;
    v->addr = (uint64_t)addr;
  }
}
