import cmd
import Queue

#======================================================
class CTPConsole(cmd.Cmd):
  """Simple command processor example."""
  
  def __init__(self, queue):
    cmd.Cmd.__init__(self)
    self.queue = queue

  def Send(self, command, params):
    self.queue.put({'c':command, 'p':params, 'name':'cp_comunication.Command'})

  def do_EOF(self, line):
    return True

  def do_exit(self, line):
    print('exit')
    #self.Send('mq')
    return True
  #login================================================
  def do_login(self, account):
    if account:
      print('login', account)
      self.Send('login', [account])
    else:
      print('login command need account')
  
  def help_login(self):
    print('\n'.join([ 'login account',
                    'example: login test',
                    ]))
  
  #mq================================================
  def do_mq(self, line):
    print('mq')
    self.Send('mq', [])

  
  def help_mq(self):
    print('\n'.join([ 'mq',
                    'example: mq',
                    ]))
  #open================================================
  def do_open(self, line):
    params = line.split()
    if len(params) == 3:
      print('open', params)
      self.Send('open', params)
    else:
      print('open command need 3 params')
  
  def help_open(self):
    print('\n'.join([ 'open account type number',
                    'example: open test IF1309 16',
                    ]))
  #close================================================
  def do_close(self, line):
    params = line.split()
    if len(params) == 3:
      print('close', params)
      self.Send('close', params)
    else:
      print('close command need 3 params')
  
  def help_close(self):
    print('\n'.join([ 'close account type number',
                    'example: close test IF1309 16',
                    ]))

  #clean================================================
  def do_clean(self, line):
    params = line.split()
    if len(params) == 2:
      print('clean', params)
      self.Send('clean', params)
    else:
      print('clean command need 2 params')
  
  def help_clean(self):
    print('\n'.join([ 'clean account type',
                    'example: clean test IF1309',
                    ]))
  #postype================================================
  def do_postype(self, line):
    params = line.split()
    if len(params) == 2:
      print('postype', params)
      self.Send('postype', params)
    else:
      print('postype command need 2 params')
  
  def help_postype(self):
    print('\n'.join([ 'postype account type',
                    'example: postype test IF1309',
                    ]))

  #pos================================================
  def do_pos(self, line):
    params = line.split()
    if len(params) == 1:
      print('pos', params)
      self.Send('pos', params)
    else:
      print('pos command need 1 params')
  
  def help_pos(self):
    print('\n'.join([ 'pos account',
                    'example: pos test',
                    ]))
#======================================================


if __name__ == '__main__':

  CTPConsole(Queue.Queue()).cmdloop()