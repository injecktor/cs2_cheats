#include <iostream>
#include <windows.h>

#include "memtool/memtool.hpp"
#include "offsets.hpp"
#include "client_dll.hpp"

using namespace mem_tool;
using namespace std;
using namespace cs2_dumper;

struct entity_t {
    DWORD hp;
    LPSTR name;
    DWORD team;
    PBYTE addr;
};

LPCTSTR cs2_name = _T("cs2.exe");

shared_ptr<PROCESSENTRY32W> process;
shared_ptr<MODULEENTRY32W> client_module;
HANDLE process_handle;

entity_t local_entity;

void get_process_info() {
    process = find_process(cs2_name);
    process_handle = OpenProcess(PROCESS_ALL_ACCESS, NULL, process->th32ProcessID);
    client_module = find_module(process->th32ProcessID, _T("client.dll"));
}

void get_local_entity() {
    read_mem(process_handle, client_module->modBaseAddr + offsets::client_dll::dwLocalPlayerPawn, 
        sizeof(PBYTE), &local_entity.addr);
    read_mem(process_handle, local_entity.addr + schemas::client_dll::C_BaseEntity::m_iHealth,
        sizeof(PBYTE), &local_entity.hp);
    read_mem(process_handle, local_entity.addr + schemas::client_dll::C_BaseEntity::m_iTeamNum,
        sizeof(PBYTE), &local_entity.team);
}

void get_entity_info(size_t id) {
    if (id > 20) {
        return;
    }
    printf("\n");
}

void get_entity_list() {
    printf("index\thealth\tarmor\tarea\n");
    for (size_t i = 0; i < 64; i++) {
        get_entity_info(i);
    }
    printf("\n");
}

int main() {
    get_process_info();
    while (true) {
        get_local_entity();
        get_entity_list();
        Sleep(1000);
    }
    return 0;
}