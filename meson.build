project('gn', 'c', version: '1.0.0')

c_flags = [
    '-Werror',
    '-Wno-padded',
    '-D_GNU_SOURCE'
]

comn_files = files(
    'src/comn/main.c',
    'src/comn/gn_close_serv_socks.c',
    'src/comn/gn_serv_sock_list_pop.c',
    'src/comn/gn_serv_sock_list_push_back.c'
)

mstr_files = files(
    'src/mstr/gn_ipc_acpt.c',
    'src/mstr/gn_mstr_main.c',
    'src/mstr/gn_open_serv_sock.c',
    'src/mstr/gn_send_serv_sock.c',
    'src/mstr/gn_start_wrkr.c'
)

wrkr_files = files(
    'src/wrkr/gn_acpt_conn.c',
    'src/wrkr/gn_acpt_conns.c',
    'src/wrkr/gn_ipc_conn.c',
    'src/wrkr/gn_recv_serv_sock.c',
    'src/wrkr/gn_recv_serv_socks.c',
    'src/wrkr/gn_wrkr_main.c'
)

all_files = comn_files + mstr_files + wrkr_files

inc_dirs = include_directories(
    'src/comn/include',
    'src/mstr/include',
    'src/wrkr/include'
)

executable('gn', all_files,
    include_directories: inc_dirs,
    c_args: c_flags,
    link_args: [
    ],
    dependencies: [
    ]
)