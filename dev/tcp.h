#include <inttypes.h>
/* size of tcpheader: 20 bytes


 */
struct tcpheader {
    uint16_t tcph_src_port;       // sourse port
    uint16_t tcph_des_port;       // destination port
    uint32_t tcph_seq_num;        // sequence number
    uint32_t tcph_ack_num;        // acknowledge number
    uint16_t
        tcph_offset   : 4, // data offset
        tcph_reserved : 6, // reserved
        tcph_urg      : 1, // urgent
        tcph_ack      : 1, // acknowledge
        tcph_psh      : 1, // push
        tcph_rst      : 1, // reset
        tcph_syn      : 1, // synchronize
        tcph_fin      : 1; // finish
    uint16_t tcph_win;            // window
    uint16_t tcph_chk_sum;        // check sum
    uint16_t tcph_urg_ptr;        // urgent pointer
};

