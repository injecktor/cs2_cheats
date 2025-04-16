#include <iostream>
#include <windows.h>

#include "memtool/memtool.hpp"
#include "offsets.hpp"

using namespace mem_tool;
using namespace std;

LPCTSTR cs2_name = _T("cs2.exe");

shared_ptr<PROCESSENTRY32W> process;
shared_ptr<MODULEENTRY32W> server_dll;
HANDLE process_handle;

PBYTE array_entry;
DWORD64 entry_point;

void get_process_info() {
    process = find_process(cs2_name);
    process_handle = OpenProcess(PROCESS_ALL_ACCESS, NULL, process->th32ProcessID);
    server_dll = find_module(process->th32ProcessID, _T("server.dll"));
}

void get_entity_info(size_t id) {
    if (id < 1 || id > 64) {
        return;
    }
    DWORD64 index = id * 0x78;

    auto entity_handle = reinterpret_cast<DWORD64>(array_entry) + index;
    size_t check_validation;
    read_mem(process_handle, reinterpret_cast<PBYTE>(entity_handle) + offsets::entity_handle::validation, 8, &check_validation);
    check_validation &= 0x7ff;
    if (check_validation != id) {
        return;
    }
    read_mem(process_handle, reinterpret_cast<PBYTE>(entity_handle), 8, &entity_handle);

    DWORD64 entity;
    read_mem(process_handle, reinterpret_cast<PBYTE>(entity_handle) + offsets::entity_handle::entity, 8, &entity);
    entity &= 0x1ff;
    entity *= 0x78;
    entity += entry_point;
    read_mem(process_handle, reinterpret_cast<PBYTE>(entity), 8, &entity);
    read_mem(process_handle, reinterpret_cast<PBYTE>(entity) + offsets::entity::offset_1, 8, &entity);
    read_mem(process_handle, reinterpret_cast<PBYTE>(entity) + offsets::entity::offset_2, 8, &entity);

    DWORD64 entity_property = 0;
    read_mem(process_handle, reinterpret_cast<PBYTE>(entity) + offsets::entity::is_dead, 1, &entity_property);
    /*if (entity_property) {
        return;
    }*/
    entity_property = 0;
    read_mem(process_handle, reinterpret_cast<PBYTE>(entity) + offsets::entity::team, 1, &entity_property);
    /*if (entity_property == 3) {
        return;
    }*/
    read_mem(process_handle, reinterpret_cast<PBYTE>(entity) + offsets::entity::hp, 4, &entity_property);
    /*if (entity_property == 0) {
        return;
    }*/
    printf("%llu\t", id);
    printf("%lu\t", static_cast<DWORD>(entity_property));
    read_mem(process_handle, reinterpret_cast<PBYTE>(entity) + offsets::entity::armor, 4, &entity_property);
    printf("%lu\t", static_cast<DWORD>(entity_property));

    char area[0xc];
    read_mem(process_handle, reinterpret_cast<PBYTE>(entity) + offsets::entity::area_string, 0xC, area);
    printf("%s\t", area);

    printf("\n");
}

void get_all_entity() {
    read_mem(process_handle, server_dll->modBaseAddr + 0x15ee268, 8, &array_entry);
    read_mem(process_handle, array_entry + 0x10, 8, &array_entry);
    read_mem(process_handle, server_dll->modBaseAddr + 0x149dba0, 8, &entry_point);
    read_mem(process_handle, reinterpret_cast<PBYTE>(entry_point), 8, &entry_point);
    printf("index\thealth\tarmor\tarea\n");
    for (size_t i = 01; i <= 64; i++) {
        get_entity_info(i);
    }
    printf("\n");
}

int main() {
    get_process_info();
    while (true) {
        get_all_entity();
        Sleep(1000);
    }
    return 0;
}