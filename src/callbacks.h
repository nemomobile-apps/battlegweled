#if HGW_FUNC
extern HgwContext *hgw_context;
#endif

int exit_callback(int errcode/*, AppData *app_data*/);
int quit_callback(int errcode/*, AppData *app_data*/);
int flush_callback(int errcode);
