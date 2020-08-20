import os
import idautils
import idaapi
import idc
import traceback
import json
import rpyc
from rpyc.utils.server import ThreadedServer

from binkit.viewer import *
from binkit.threadtool import *

class BinKitService(rpyc.Service):
    def on_connect(self, conn):
        self.ida = IDA()

    def get_pid(self):
        return os.getpid()
    
    def jumpto(self, address):
        self.ida.jumpto(address)
        
    def get_md5(self):
        return self.ida.get_md5()

def start_binkit_server(connection_filename):
    port = 18861
    while 1:
        try:
            t = ThreadedServer(BinKitService(), port = port, protocol_config = {
                'allow_public_attrs': True,
            })
            print('Listening on %d\n' % port)
            
            md5 = idc.GetInputMD5().lower()
            try:
                with open(connection_filename, "w") as fd:
                    json.dump({'port': port, 'md5': md5}, fd)
            except:
                traceback.print_exc()

            t.start()
            break
        except:
            port += 1
            traceback.print_exc()

class BinkitPlugin(idaapi.plugin_t):
    flags = idaapi.PLUGIN_KEEP
    comment = "Binkit Sync Agent"

    wanted_name = "Binkit Agent Plugin"
    wanted_hotkey = "Alt-F6"
    help = "TestPlugin..."

    def init(self):
        self.get_connection_filename()
        thread.start_new_thread(start_binkit_server, (self.connection_filename,))  
        return idaapi.PLUGIN_KEEP

    def get_connection_filename(self):
        binkit_profile = os.path.join(os.environ['USERPROFILE'], '.binkit')
        if not os.path.isdir(binkit_profile):
            try:
                print("Creating %s" % binkit_profile)
                os.makedirs(binkit_profile)
            except:
                traceback.print_exc()
            
        md5 = idc.GetInputMD5().lower()
        self.connection_filename = os.path.join(binkit_profile, "%s-%d.port" % (md5, os.getpid()))

    def run(self, arg):
        viewer = Viewer(get_filename())
        viewer.show_functions_match_viewer()
        viewer.set_basic_blocks_color(0xCCFFFF, 0xCC00CC)
        idaapi.set_dock_pos("Function Matches", "Functions window", idaapi.DP_TAB)

    def term(self):
        if os.path.isfile(self.connection_filename):
            try:
                print("Removing %s" % self.connection_filename)
                os.remove(self.connection_filename)
            except:
                traceback.print_exc()

def PLUGIN_ENTRY():
    return BinkitPlugin()
