%include {
#include <assert.h>
#include <stdlib.h>
#include "amiws.h"

#include <stdio.h>

}

%parse_failure {
  pConf->parse_fail = 1;
}
%syntax_error {
  pConf->syntax_error = 1;
}

%extra_argument { struct amiws_config *pConf }

%token_type { char * }

yaml ::= STREAM_START block STREAM_END .

block ::= BLOCK_MAPPING_START conf connections BLOCK_END .

conf ::= conf field .
conf ::= .

field     ::= key(K) value(V) . {
  set_conf_param(pConf, K, V);
}
key(A)    ::= KEY SCALAR(S)   . { A = S; }
value(A)  ::= VALUE SCALAR(S) . { A = S; }

connections ::= key(A) VALUE BLOCK_SEQUENCE_START ami_servers BLOCK_END . { free(A); }
ami_servers ::= ami_servers server .
ami_servers ::= .
server ::= BLOCK_ENTRY BLOCK_MAPPING_START entries BLOCK_END .
entries ::= entries entry_field .
entries ::= . {
  struct amiws_conn *conn;
  macro_init_conn(conn);
#if MG_ENABLE_SSL
  conn->ssl_cert      = NULL;
  conn->ssl_key       = NULL;
#endif
  if (pConf->size == 0) {
    pConf->head = conn;
    pConf->tail = conn;
    conn->next = NULL;
  } else {
    pConf->tail->next = conn;
    pConf->tail = conn;
  }
  pConf->size++;
  conn->id = pConf->size;
}
entry_field ::= entry_key(K) entry_value(V) . {
  set_conn_param(pConf->tail, K, V);
}
entry_key(A)    ::= KEY SCALAR(S)   . { A = S; }
entry_value(A)  ::= VALUE SCALAR(S) . { A = S; }

