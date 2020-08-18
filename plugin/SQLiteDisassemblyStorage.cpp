#pragma once
#include <stdio.h>
#include <string>
#include <unordered_set>
#include <map>
#include <iostream>

using namespace std;
using namespace stdext;

#include "sqlite3.h"

#include "StorageDataStructures.h"
#include "SQLiteDisassemblyStorage.h"

#include "Log.h"

SQLiteDisassemblyStorage::SQLiteDisassemblyStorage(const char *DatabaseName)
{
    m_database = NULL;
    if (DatabaseName)
    {
        m_sqliteTool.Open(DatabaseName);
        CreateTables();
    }
}

SQLiteDisassemblyStorage::~SQLiteDisassemblyStorage()
{
    m_sqliteTool.Close();
}

void SQLiteDisassemblyStorage::CreateTables()
{
    m_sqliteTool.ExecuteStatement(NULL, NULL, CREATE_BASIC_BLOCKS_TABLE_STATEMENT);
    m_sqliteTool.ExecuteStatement(NULL, NULL, CREATE_BASIC_BLOCKS_TABLE_FUNCTION_ADDRESS_INDEX_STATEMENT);
    m_sqliteTool.ExecuteStatement(NULL, NULL, CREATE_BASIC_BLOCKS_TABLE_START_ADDRESS_INDEX_STATEMENT);
    m_sqliteTool.ExecuteStatement(NULL, NULL, CREATE_BASIC_BLOCKS_TABLE_END_ADDRESS_INDEX_STATEMENT);
    m_sqliteTool.ExecuteStatement(NULL, NULL, CREATE_CONTROL_FLOWS_TABLE_STATEMENT);
    m_sqliteTool.ExecuteStatement(NULL, NULL, CREATE_CONTROL_FLOWS_TABLE_SRCBLOCK_INDEX_STATEMENT);
    m_sqliteTool.ExecuteStatement(NULL, NULL, CREATE_BINARIES_TABLE_STATEMENT);
}

bool SQLiteDisassemblyStorage::Open(char *databaseName)
{
    return m_sqliteTool.Open(databaseName);
}

int SQLiteDisassemblyStorage::BeginTransaction()
{
    return m_sqliteTool.BeginTransaction();
}

int SQLiteDisassemblyStorage::EndTransaction()
{
    return m_sqliteTool.EndTransaction();
}

void SQLiteDisassemblyStorage::SetBinaryMetaData(BinaryMetaData *pBinaryMetaData, int fileID)
{
    LogMessage(2, __FUNCTION__, "OriginalFilePath: %s\n", pBinaryMetaData->OriginalFilePath);
    LogMessage(2, __FUNCTION__, "MD5: %s\n", pBinaryMetaData->MD5.c_str());
    LogMessage(2, __FUNCTION__, "SHA256: %s\n", pBinaryMetaData->SHA256.c_str());
    m_sqliteTool.ExecuteStatement(NULL, NULL, INSERT_BINARIES_TABLE_STATEMENT,
        fileID,
        pBinaryMetaData->OriginalFilePath,
        pBinaryMetaData->MD5.c_str(),
        pBinaryMetaData->SHA256.c_str()
    );
}

void SQLiteDisassemblyStorage::AddBasicBlock(BasicBlock &basicBlock, int fileID)
{
    m_sqliteTool.ExecuteStatement(NULL, NULL, INSERT_BASIC_BLOCKS_TABLE_STATEMENT,
        fileID,
        basicBlock.StartAddress,
        basicBlock.EndAddress,
        basicBlock.Flag,
        basicBlock.FunctionAddress,
        basicBlock.BlockType,
        basicBlock.Name.c_str(),
        basicBlock.DisasmLines.c_str(),
        basicBlock.InstructionHash.c_str(),
        basicBlock.InstructionBytes.c_str()
    );
}

void SQLiteDisassemblyStorage::AddControlFlow(ControlFlow &controlFlow, int fileID)
{
    m_sqliteTool.ExecuteStatement(NULL, NULL, INSERT_CONTROL_FLOWS_TABLE_STATEMENT,
        fileID,
        controlFlow.Type,
        controlFlow.SrcBlock,
        controlFlow.SrcBlockEnd,
        controlFlow.Dst
    );
}
