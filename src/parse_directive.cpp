#include <cstdio>
#include "common.h"
#include "buffer.h"
#include "error.h"
#include "parser.h"

void TParser::ParseDirective(TSymtabNode *pRoutineId) {

    switch (token) {
        case tcInclude:
        {
            GetToken();
            TParser *parser = new TParser(new TSourceBuffer(pToken->String()));

            TSymtabNode *pModule = parser->Parse();
            delete parser;
            GetToken();
        }
        break;
    }

}