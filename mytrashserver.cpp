#include "mytrashserver.h"

int main()
{
	::signal(SIGCHLD,SIG_IGN);
	myTrashServer srv;
	srv.accept();
	return 0;
}
