/*
 *   S3DL - alphabet.c
 *   Written by Rocco Marco Guglielmi, font created by Marisa Monaco
 */

#include "../../userlib/include/alphabet.h"


uint8_t ascii_8x8_table[128][ASCII_DIM] = {
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* NULL*/
                                          {0xFC, 0xFE, 0x06, 0x0C, 0x38, 0x60, 0x60, 0x3F}, /*    S*/
                                          {0xFF, 0xFF, 0x18, 0x18, 0x18, 0x0C, 0x0C, 0x0C}, /*    T*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  ETX*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  EOT*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  ENQ*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  ACK*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  BEL*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*   BS*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  TAB*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*   LF*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*   VT*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*   FF*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*   CR*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*   SO*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*   SI*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  DLE*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  DC1*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  DC2*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  DC3*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  DC4*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  NAK*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  SYN*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  ETB*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  CAN*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*   EM*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  SUB*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*  ESC*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*   FS*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*   GS*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*   RS*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*   US*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*     */
                                          {0x60, 0x60, 0x60, 0x60, 0x60, 0x00, 0x60, 0x60}, /*    !*/
                                          {0x6C, 0x6C, 0x48, 0x24, 0x00, 0x00, 0x00, 0x00}, /*    "*/
                                          {0x14, 0x14, 0x7F, 0x14, 0x14, 0x7F, 0x14, 0x14}, /*    #*/
                                          {0x08, 0x7E, 0x09, 0x09, 0x3E, 0x48, 0x48, 0x3F}, /*    $*/
                                          {0x02, 0x45, 0x22, 0x10, 0x08, 0x24, 0x52, 0x20}, /*    %*/
                                          {0x04, 0x0A, 0x0A, 0x04, 0x0A, 0x51, 0x21, 0x5E}, /*    &*/
                                          {0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00}, /*    '*/
                                          {0x40, 0x20, 0x10, 0x10, 0x10, 0x10, 0x20, 0x40}, /*    (*/
                                          {0x01, 0x02, 0x04, 0x04, 0x04, 0x04, 0x02, 0x01}, /*    )*/
                                          {0x00, 0x49, 0x2A, 0x1C, 0x08, 0x1C, 0x2A, 0x49}, /*    **/
                                          {0x18, 0x18, 0x18, 0xFF, 0xFF, 0x18, 0x18, 0x18}, /*    +*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x01}, /*    ,*/
                                          {0x00, 0x00, 0x00, 0XFE, 0XFE, 0x00, 0x00, 0x00}, /*    -*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03}, /*    .*/
                                          {0x04, 0x04, 0x04, 0x02, 0x02, 0x01, 0x01, 0x01}, /*    /*/
                                          {0x3E, 0x7F, 0x63, 0x63, 0x63, 0x63, 0x7F, 0x3E}, /*    0*/
                                          {0x1C, 0x1E, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x3E}, /*    1*/
                                          {0x3E, 0x7F, 0x63, 0x60, 0x3E, 0x03, 0x7F, 0x7F}, /*    2*/
                                          {0x7F, 0x7F, 0x38, 0x1C, 0x38, 0x63, 0x7F, 0x3E}, /*    3*/
                                          {0x38, 0x3C, 0x36, 0x33, 0x7F, 0x7F, 0x30, 0x30}, /*    4*/
                                          {0x7F, 0x7F, 0x03, 0x3F, 0x60, 0x63, 0x7F, 0x3E}, /*    5*/
                                          {0x3E, 0x7F, 0x03, 0x3F, 0x63, 0x63, 0x7F, 0x3E}, /*    6*/
                                          {0x7F, 0x7F, 0x30, 0x30, 0x18, 0x18, 0x0C, 0x0C}, /*    7*/
                                          {0x3E, 0x63, 0x63, 0x3E, 0x63, 0x63, 0x7F, 0x3E}, /*    8*/
                                          {0x3E, 0x7F, 0x63, 0x63, 0x7E, 0x60, 0x7F, 0x3E}, /*    9*/
                                          {0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00}, /*    :*/
                                          {0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x02, 0x01}, /*    ;*/
                                          {0x00, 0x38, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x38}, /*    <*/
                                          {0x00, 0x00, 0x00, 0x7F, 0x00, 0x7F, 0x00, 0x00}, /*    =*/
                                          {0x00, 0x0E, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0E}, /*    >*/
                                          {0x1C, 0x22, 0x22, 0x10, 0x08, 0x08, 0x00, 0x08}, /*    ?*/
                                          {0x18, 0x24, 0x32, 0x2A, 0x2A, 0x1A, 0x04, 0x38}, /*    @*/
                                          {0x3E, 0x7F, 0x63, 0x63, 0x7F, 0x7F, 0x63, 0x63}, /*    A*/
                                          {0x3F, 0x7F, 0x63, 0x3F, 0x63, 0x63, 0x7F, 0x3F}, /*    B*/
                                          {0x7E, 0x7F, 0x03, 0x03, 0x03, 0x03, 0x7F, 0x7E}, /*    C*/
                                          {0x3F, 0x7F, 0x63, 0x63, 0x63, 0x63, 0x7F, 0x3F}, /*    D*/
                                          {0x7F, 0x7F, 0x03, 0x3F, 0x3F, 0x03, 0x7F, 0x7F}, /*    E*/
                                          {0x7F, 0x7F, 0x03, 0x3F, 0x3F, 0x03, 0x03, 0x03}, /*    F*/
                                          {0x3E, 0x7F, 0x03, 0x03, 0x73, 0x63, 0x7F, 0x3E}, /*    G*/
                                          {0x63, 0x63, 0x63, 0x7F, 0x7F, 0x63, 0x63, 0x63}, /*    H*/
                                          {0x3E, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x3E}, /*    I*/
                                          {0x7F, 0x7F, 0x30, 0x30, 0x33, 0x33, 0x3F, 0x1E}, /*    J*/
                                          {0x33, 0x3B, 0x1F, 0x0F, 0x1F, 0x3B, 0x73, 0x63}, /*    K*/
                                          {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x7F, 0x7F}, /*    L*/
                                          {0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x63}, /*    M*/
                                          {0x63, 0x67, 0x6F, 0x7F, 0x7B, 0x73, 0x63, 0x63}, /*    N*/
                                          {0x3E, 0x7F, 0x63, 0x63, 0x63, 0x63, 0x7F, 0x3E}, /*    O*/
                                          {0x3F, 0x7F, 0x63, 0x63, 0x7F, 0x3F, 0x03, 0x03}, /*    P*/
                                          {0x3E, 0x7F, 0x63, 0x63, 0x6B, 0x73, 0x7F, 0x3E}, /*    Q*/
                                          {0x3F, 0x7F, 0x63, 0x63, 0x3F, 0x1F, 0x3B, 0x73}, /*    R*/
                                          {0x7E, 0x7F, 0x03, 0x3F, 0x7E, 0x60, 0x7F, 0x3F}, /*    S*/
                                          {0x7F, 0x7F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C}, /*    T*/
                                          {0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x7F, 0x3E}, /*    U*/
                                          {0x63, 0x63, 0x63, 0x63, 0x36, 0x36, 0x1C, 0x1C}, /*    V*/
                                          {0x63, 0x63, 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x36}, /*    W*/
                                          {0x63, 0x63, 0x77, 0x3E, 0x1C, 0x3E, 0x77, 0x63}, /*    X*/
                                          {0x63, 0x63, 0x36, 0x3E, 0x1C, 0x1C, 0x1C, 0x1C}, /*    Y*/
                                          {0x7F, 0x7F, 0x38, 0x1C, 0x0E, 0x07, 0x7F, 0x7F}, /*    Z*/
                                          {0xE0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xE0}, /*    [*/
                                          {0x80, 0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10}, /*    \*/
                                          {0xE0, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xE0}, /*    ]*/
                                          {0x10, 0x28, 0x44, 0x82, 0x00, 0x00, 0x00, 0x00}, /*    ^*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF}, /*    _*/
                                          {0x20, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00}, /*    `*/
                                          {0x00, 0x00, 0x3C, 0x60, 0x7C, 0x66, 0x7C, 0x00}, /*    a*/
                                          {0x00, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3E, 0x00}, /*    b*/
                                          {0x00, 0x00, 0x3C, 0x06, 0x06, 0x06, 0x3C, 0x00}, /*    c*/
                                          {0x00, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x7C, 0x00}, /*    d*/
                                          {0x00, 0x00, 0x3C, 0x66, 0x7E, 0x06, 0x3C, 0x00}, /*    e*/
                                          {0x00, 0x70, 0x18, 0x7C, 0x18, 0x18, 0x18, 0x00}, /*    f*/
                                          {0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x3E}, /*    g*/
                                          {0x00, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x66, 0x00}, /*    h*/
                                          {0x00, 0x18, 0x00, 0x1C, 0x18, 0x18, 0x3C, 0x00}, /*    i*/
                                          {0x00, 0x60, 0x00, 0x60, 0x60, 0x60, 0x60, 0x3C}, /*    j*/
                                          {0x00, 0x06, 0x06, 0x36, 0x1E, 0x36, 0x66, 0x00}, /*    k*/
                                          {0x00, 0x1C, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00}, /*    l*/
                                          {0x00, 0x00, 0x66, 0xFE, 0xFE, 0xD6, 0xC6, 0x00}, /*    m*/
                                          {0x00, 0x00, 0x3E, 0x66, 0x66, 0x66, 0x66, 0x00}, /*    n*/
                                          {0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00}, /*    o*/
                                          {0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x06}, /*    p*/
                                          {0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60}, /*    q*/
                                          {0x00, 0x00, 0x3E, 0x66, 0x06, 0x06, 0x06, 0x00}, /*    r*/
                                          {0x00, 0x00, 0x7C, 0x06, 0x3C, 0x60, 0x3E, 0x00}, /*    s*/
                                          {0x00, 0x18, 0x7E, 0x18, 0x18, 0x18, 0x70, 0x00}, /*    t*/
                                          {0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x00}, /*    u*/
                                          {0x00, 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00}, /*    v*/
                                          {0x00, 0x00, 0xC6, 0xD6, 0xFE, 0x7C, 0x6C, 0x00}, /*    w*/
                                          {0x00, 0x00, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x00}, /*    x*/
                                          {0x00, 0x00, 0x66, 0x66, 0x66, 0x7C, 0x30, 0x1E}, /*    y*/
                                          {0x00, 0x00, 0x7E, 0x30, 0x18, 0x0C, 0x7E, 0x00}, /*    z*/
                                          {0x0C, 0x02, 0x02, 0x01, 0x02, 0x02, 0x02, 0x0C}, /*    {*/
                                          {0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08}, /*    |*/
                                          {0x18, 0x20, 0x20, 0x40, 0x20, 0x20, 0x20, 0x18}, /*    }*/
                                          {0x00, 0x00, 0x00, 0x40, 0x3E, 0x01, 0x00, 0x00}, /*    ~*/
                                          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*   DEL*/
                                          };