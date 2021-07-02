/**
 * Extreme Edge Cases
 * CS 241 - Spring 2020
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camelCaser.h"
#include "camelCaser_tests.h"

int test_camelCaser(char **(*camelCaser)(const char *),
                    void (*destroy)(char **)) {
    // TODO: Implement me!
    //char* s = "!#$&()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    //char* testString = "The Heisenbug is an incredible creature. Facenovel servers get their power from its indeterminism. Code smell can be ignored with INCREDIBLE use of air freshener. God objects are the new religion.";
    //char** output = camelCaser(testString);
    char * inputs = "2we45ry . ehwsbu f ! eifoiejfASWE di3edrf \3 teSTING... wow!!!";
    char * corr_outputs[] = {"2we45ry",
	"ehwsbuF",
	"eifoiejfasweDi3edrfTesting",
    "",
    "",
	"wow",
	"",
	"",
	NULL
};  char ** corr = corr_outputs;
    char** output = camelCaser(NULL);
    if (output != NULL) {
        return 0;
    }
    output = camelCaser(inputs);
    while(*corr) {
        printf("%s\n%s\n", *output, *corr);
        output++;
        corr++;
        if(strcmp(*output, *corr) != 0) {
            //printf("%s\n%s\n", *output, *corr);
            return 0;
        }
    }

    //int index = 0;
    printf("success");
    destroy(output);
    return 1;
}
