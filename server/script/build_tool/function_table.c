#include <sys/types.h>
#include <dirent.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "sym_parser.h"

#define SCRIPT_FUNCTION_HEADER_STRING "ATM_"
#define ATM_STRING_FUNCTION "function"
#define ATM_SOURCEFILE_SUFFIX ".script"
const int ATM_MAX_FILENAME_LENGTH = 50;
const int ATM_MAX_PATH_LENGTH = 200;

//所有脚本的总字节数上限1M
const int ATM_MAX_SOURCE_LENGTH = 1024000;
const int ATM_SECTION_STRING_LENGTH = 16;
const int ATM_METHOD_STRING_LENGTH = 64;
const int ATM_MAX_SENTENCE_LENGTH = 1024;

//脚本SO内存空间字节数10M
const int ATM_MAX_SCRIPT_SO_SIZE = 10240000;

int loadscriptfile(char* filename, FILE *file_code, FILE *file_reg)
{
	assert(filename);
	assert(file_code);
	assert(file_reg);	

	int ret = -1;

    uint8_t type;
    uint32_t type_param;
	ret = parse_file_name(filename, &type, &type_param);
	if(ret < 0)
	{
		//文件名非法
		return -1;
	}

	char line[ATM_MAX_SENTENCE_LENGTH];
	line[0] = '\0';
	uint8_t end = 0;

	FILE* file = fopen(filename, "rb");
	if(!file)
	{
		return -2;
	}

	uint8_t action;
	uint32_t action_param;

	fprintf(file_code, "#line 0 \"%s\"\n", filename);
	
	while(!end)
	{
		line[0] = '\0';
		fgets(line, sizeof(line), file);

		if(feof(file))
		{
			end = 1;
		}

		ret = strncmp(line, ATM_STRING_FUNCTION, sizeof(ATM_STRING_FUNCTION) - 1);
		if(ret != 0)
		{
			//不是function开头的函数定义行
			//printf("ATM_STRING_FUNCTION %s %s %d %d \n", szLine, ATM_STRING_FUNCTION, iRet, sizeof(ATM_STRING_FUNCTION));

            // 将 ATM_ 替换为: CScriptIsland::m_pScriptCallback->ATM_
/*
            const char* pATMToken = "ATM_";
            const char* pATMReplacement = "script_callback.";

            int start = 0;
            while (1)
            {
                char* pATM = strstr(line + start, pATMToken);
                if (!pATM)
                {
                    break;
                }

                memmove(pATM + strlen(pATMReplacement), pATM, strlen(pATM) + 1);
                memmove(pATM, pATMReplacement, strlen(pATMReplacement));


                start += pATM - line + strlen(pATMReplacement) + strlen(pATMToken);
            }
*/
//			AppendSourceBuffer(ESSB_FunctionDefinition, line);
			fprintf(file_code, "%s", line);
			continue;
		}

		ret = parse_function_name(&line[sizeof(ATM_STRING_FUNCTION)], &action, &action_param);
		if(ret < 0)
		{
			continue;
		}
		//printf("Origin =====> %s\n", szLine);

		char function_name[ATM_MAX_SENTENCE_LENGTH];
		generate_function_name_sentence(function_name, type, type_param, action, action_param);
		fprintf(file_code, "int %s()\n", function_name);
		fprintf(file_reg, "extern int %s();\n", function_name);		
		fprintf(file_reg, "register_function(%u, %u, %u, %u, %s);\n", type, type_param, action, action_param, function_name);
/*				
		//function替换为int
		memcpy(line, "\nint     ", sizeof(ATM_STRING_FUNCTION) - 1);
		line[sizeof(ATM_STRING_FUNCTION) - 1] = ' ';


        // 函数名前追加ContextSection和ContextThingID
        char* pszFunctionBegin = strstr(line, SCRIPT_FUNCTION_HEADER_STRING);
        pszFunctionBegin += sizeof(SCRIPT_FUNCTION_HEADER_STRING) - 1;

        char szContextString[100];
//        sprintf(szContextString, "%s_%d_%s_%d", get_type_name(type), type, get_action_name(action), action);
 
        memmove(pszFunctionBegin + strlen(szContextString), pszFunctionBegin, strlen(pszFunctionBegin) + 1);
        memmove(pszFunctionBegin, szContextString, strlen(szContextString));

//		AppendSourceBuffer(ESSB_FunctionDefinition, szLine);
		//printf("%s %d %d %d %d\n", szLine, iRet, iSectionID, iThingID, iMethodID);
 
        memset(line, 0, sizeof(line));

//        AppendSourceBuffer(ESSB_FunctionDefinition, szLine);

		char szSentence[ATM_MAX_SENTENCE_LENGTH];
		generate_register_sentence(szSentence, type, type_param, action, action_param);
//		AppendSourceBuffer(ESSB_FunctionRegistion, szSentence);
*/
	}

	fclose(file);

	return 0;
}

int traversescriptpath(char* path)
{
	assert(path);
//	strcpy(m_szScriptPath, pszScriptPath, sizeof(m_szScriptPath));

//	struct stat stStatBuffer;
	struct dirent* pstDirent;
	DIR* pDIR;
	FILE* file_code, *file_reg;

	file_code = fopen("../so/FunctionDefinition.c", "wb");
	if(!file_code)
	{
		return -1;
	}

	file_reg = fopen("../so/FunctionRegist.sc", "wb");
	if(!file_reg)
	{
		return -1;
	}

	fprintf(file_code, "#include <stdio.h>\n");
	fprintf(file_code, "#include <stdint.h>\n");
	fprintf(file_code, "#include <sys/types.h>\n");
	fprintf(file_code, "#include <stdlib.h>\n");
	fprintf(file_code, "#include \"script_callback.h\"\n");					
	
	char filename[ATM_MAX_FILENAME_LENGTH];

	int file_num = 0;

	int iRet = -1;
/*
	iRet = (m_szScriptPath , &stStatBuffer) < 0;
	if(iRet < 0)
	{
		printf("read path error:%s\n", m_szScriptPath);
		return -1;
	}
*/
	pDIR = opendir(path);
	if(!pDIR)
	{
		printf("can't open path\n");
		return -2;
	}

    // 分析目录下的每一个cxx后缀的脚本, 抽取函数和变量
	while((pstDirent = readdir(pDIR)) != NULL)
	{
		strncpy(filename, pstDirent->d_name, sizeof(filename));
		filename[sizeof(filename) - 1] = '\0';

		if(strlen(filename) < strlen(ATM_SOURCEFILE_SUFFIX))
		{
			continue;
		}

		iRet = strncmp(filename + strlen(filename) - strlen(ATM_SOURCEFILE_SUFFIX), ATM_SOURCEFILE_SUFFIX, sizeof(filename));
		if(iRet != 0)
		{
			continue;
		}

		file_num++;
		//printf("%s\n", szFileName);

		loadscriptfile(filename, file_code, file_reg);

	}

	closedir(pDIR);

	fclose(file_code);
	fclose(file_reg);
	
    // 分析完毕, 导出分析数据
/*
	FILE* pDumpFile;

	pDumpFile = fopen("./FunctionDefinition.atm", "wb");
	if(!pDumpFile)
	{
		return -3;
	}
//	fwrite(m_astScriptSource[ESSB_FunctionDefinition].m_szBuffer, 1, m_astScriptSource[ESSB_FunctionDefinition].m_iLength, pDumpFile);
	fclose(pDumpFile);

	pDumpFile = fopen("./FunctionRegistion.atm", "wb");
	if(!pDumpFile)
	{
		return -4;
	}
//	fwrite(m_astScriptSource[ESSB_FunctionRegistion].m_szBuffer, 1, m_astScriptSource[ESSB_FunctionRegistion].m_iLength, pDumpFile);
	fclose(pDumpFile);
*/	
	return (0);
}

int main(int argc, char *argv[])
{
	if (argc > 1)
		traversescriptpath(argv[1]);
	else
		traversescriptpath(".");
	return (0);
}

