project('gn', 'c', version: '1.0.0')

c_flags = [
    '-g',
    '-Werror',
    '-Wno-padded',
    '-D_GNU_SOURCE'
]

comn_files = files(
    'src/comn/main.c',
    'src/comn/gn_close_serv_socks.c',
    'src/comn/gn_close.c',
    'src/comn/gn_serv_sock_list_pop.c',
    'src/comn/gn_serv_sock_list_push_back.c'
)

mstr_files = files(
    'src/mstr/gn_ipc_acpt.c',
    'src/mstr/gn_load_mstr_conf.c',
    'src/mstr/gn_mstr_init.c',
    'src/mstr/gn_mstr_main.c',
    'src/mstr/gn_open_serv_sock.c',
    'src/mstr/gn_send_serv_sock.c',
    'src/mstr/gn_start_wrkr.c',
    'src/mstr/gn_start_wrkrs.c',
    'src/mstr/gn_stop_wrkrs.c',
    'src/mstr/gn_wrkr_data_list_pop.c',
    'src/mstr/gn_wrkr_data_list_push_back.c',
    'src/mstr/gn_wrkr_data_list_remove.c'
)

wrkr_files = files(
    'src/wrkr/gn_acpt_conn.c',
    'src/wrkr/gn_acpt_conns.c',
    'src/wrkr/gn_conn_acpt_thrd_data_list_push_back.c',
    'src/wrkr/gn_conn_acpt_thrd_data_list_remove.c',
    'src/wrkr/gn_conn_acpt_thrd.c',
    'src/wrkr/gn_conn_mgmt_thrd_data_list_push_back.c',
    'src/wrkr/gn_conn_mgmt_thrd_data_list_remove.c',
    'src/wrkr/gn_conn_mgmt_thrd.c',
    'src/wrkr/gn_ipc_conn.c',
    'src/wrkr/gn_recv_serv_sock.c',
    'src/wrkr/gn_recv_serv_socks.c',
    'src/wrkr/gn_start_conn_acpt_thrds.c',
    'src/wrkr/gn_start_conn_mgmt_thrds.c',
    'src/wrkr/gn_stop_conn_acpt_thrds.c',
    'src/wrkr/gn_stop_conn_mgmt_thrds.c',
    'src/wrkr/gn_wrkr_init.c'
)

all_files = comn_files + mstr_files + wrkr_files

inc_dirs = include_directories(
    'src/comn/include',
    'src/mstr/include',
    'src/wrkr/include'
)

dep_json_c = dependency('json-c')

executable('gn', all_files,
    include_directories: inc_dirs,
    c_args: c_flags,
    link_args: [
    ],
    dependencies: [
        dep_json_c
    ]
)