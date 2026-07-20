#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if __linux__
	#include <unistd.h>
	#include <sys/wait.h>
#elif _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

//#define EXIT_FAILURE -1

char *build_jar_name(const char *program_name)
{
	char* jar_file;
	static const char *suffix = ".jar";
	
	// recontruction du nom du fichier ".jar"
	jar_file = malloc((strlen(program_name) + strlen(suffix) + 1)*sizeof(char)); //len(argv[0]) + '\0'
	
	if (jar_file != NULL) {
		jar_file[0] = '\0';
#if __linux__
		// sous linux, supprime le prefixe "./" du repertoire courant
		if (strncmp(program_name, "./", 2) == 0) {
			size_t len = strlen(program_name) - 2;
			strncpy(jar_file, (char *)(program_name + 2), len);
			jar_file[len] = '\0';
		}
#elif _WIN32
		// sous windows, supprime le suffixe ".exe" pour l'executable en cours
		size_t i = strlen(program_name);
		if (i > 4) i -= 4; // preparation du curseur pour trouver le suffixe ".exe"
		if(strncmp((char *)(program_name + i), ".exe", 4) == 0) {
			strncpy(jar_file, program_name, i);
			jar_file[i]= '\0';
			}
#else
		if (0) ;
#endif
		else {
			size_t len = strlen(program_name);
			strncpy(jar_file, program_name, len);
			jar_file[len] = '\0';
		}
		strncat(jar_file, suffix, strlen(suffix));
	}
	return jar_file;
}


#if _WIN32
	int jl_exec_windows(char* cmd_string) {
		// prepare process
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		memset(&si, 0, sizeof(STARTUPINFO));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_FORCEONFEEDBACK;
		memset(&pi, 0, sizeof(PROCESS_INFORMATION));
		//const DWORD flags = CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS; // process en tache de fond
		//const DWORD flags = 0; // pour le debug
		const DWORD flags = DETACHED_PROCESS; // pour la version finale
		// run process
		if (!CreateProcess(NULL, cmd_string, NULL, NULL, FALSE, flags, NULL, NULL, &si, &pi)) {
		//if (!CreateProcess("java.exe", cmd_string, NULL, NULL, FALSE, flags, NULL, NULL, &si, &pi)) {
			printf("CreateProcess failed (%lu)\n", GetLastError());
			return EXIT_FAILURE;
		}
		else {
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			return EXIT_SUCCESS;
		}
	}
#endif


int main(int argc, char *argv[]) {
	// parent path
	char* jar_file;
	static const char *jar_option = "-jar";

	if (argc == 0) return EXIT_FAILURE;

#if __linux__
	const char *java_cmd = "java";
	pid_t pid = fork();
	
	if (pid < 0) {
		perror("fork");
		return EXIT_FAILURE;
	}
	
	if (pid == 0) {
		// processus fils
		jar_file = build_jar_name(argv[0]);
		
		if (jar_file != NULL) {
			//printf("jar_file : %s\n", jar_file);
			// liste des arguments pour la function "execvp"
			//char **args = malloc((argc + 3) * sizeof(*args));
			char **args = malloc((argc + 3) * sizeof(char *));
			
			if (args != NULL) {
				args[0] = (char *) java_cmd;   // "java"
				args[1] = (char *) jar_option; // "-jar"
				args[2] = jar_file;   // jar filename

				// recopie des arguments suivants
				for (int i = 1; i < argc; i++)
					args[i + 2] = argv[i];

				// dernier argument obligatoirement NULL
				args[argc + 2] = NULL;

				execvp("java", args);
				//for (int i = 0; i < (argc + 2); i++)
				//	printf("%s\n", args[i]);

				perror("execvp");
				free(args);
				free(jar_file);
				_exit(EXIT_FAILURE);
			}
			free(jar_file);
		}
		//printf("Done !\n");
		return EXIT_SUCCESS;
	}
	else {
		// processus parent
		//wait(NULL);
		int status;
		int retVal;
		retVal = waitpid(pid, &status, 0);
		if (retVal == -1) {
			perror("waitpid");
			return EXIT_FAILURE;
		}
		if (WIFEXITED(status))
			return WEXITSTATUS(status);
		return EXIT_FAILURE;
	}
#elif _WIN32
	const char *java_cmd = "java.exe";
	char* java_path;
	
	size_t length_str = 0;

	jar_file = build_jar_name(argv[0]);
	
	if (jar_file != NULL) {
		//printf("Launching : java.exe -jar '%s.jar'\n", jar_file);
		
		// allocation de la mémoire pour la chaîne destination
		length_str = strlen(java_cmd) + 1;   // to store the string "java.exe" + one space
		length_str += strlen(jar_option) + 1; // to store the string "-jar" + one space
		length_str += strlen(jar_file);      // to store the jar file name
		if (strchr(jar_file, ' ') != NULL)
			length_str += 2;                 // to store the string double quote
		if (argc > 0) {
			for (int i = 1; i < argc; i++) {
				if (strchr(argv[i], ' ') != NULL)
					length_str += 2;               // to store the string double quote
				length_str += strlen(argv[i]) + 1; // store le argv string + one space character
			}
		}
		length_str += 1; // to store '\0' (end of string)
		
		//printf("%s [%d]\n", java_cmd, strlen(java_cmd));
		//printf("%s [%d]\n", jar_option, strlen(jar_option));
		//printf("%s [%d]\n", jar_file, strlen(jar_file));
		//for (int i = 1; i < argc; i++)
		//	printf("%s [%d]\n", argv[i], strlen(argv[i]));
		
		java_path = malloc(length_str*sizeof(char));
		if (java_path != NULL) {
			// prepare the command string
			java_path[0] = '\0'; // init string java_path
			strncat(java_path, java_cmd, strlen(java_cmd));
			strncat(java_path, " ", 1);
			strncat(java_path, jar_option, strlen(jar_option));
			strncat(java_path, " ", 1);
			if (strchr(jar_file, ' ') != NULL) {
				strncat(java_path, "\"", 1);
				strncat(java_path, jar_file, strlen(jar_file));
				strncat(java_path, "\"", 1);
			}
			else {
				strncat(java_path, jar_file, strlen(jar_file));
			}
			if (argc > 0) {
				// prepare the list of arguments
				for (int i = 1; i < argc; i++)
				{
					strncat(java_path, " ", 1);
					if (strchr(argv[i], ' ') != NULL)
					{
						strncat(java_path, "\"", 1);
						strncat(java_path, argv[i], strlen(argv[i]));
						strncat(java_path, "\"", 1);
					}
					else
						strncat(java_path, argv[i], strlen(argv[i]));
				}
			}
			// execute the command line into the system
			int retVal = jl_exec_windows(java_path);
			
			//printf("Launching : '%s'\n", java_path);
			//int retVal = 0;
			
			free (java_path);
			free (jar_file);
			return retVal;
		}
		else {
			free (jar_file);
			return EXIT_FAILURE;
		}
		
	}
	// free memory
	free (jar_file);
	return EXIT_SUCCESS;
#endif
}
