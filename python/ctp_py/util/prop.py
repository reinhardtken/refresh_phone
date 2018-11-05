import utility


class Prop:

  def __init__(self, prop):
    if prop != None:
      self.content = prop
    else:
      self.content = {}


  def __getitem__(self, key):
    if self.content.has_key(key):
      return self.content[key]

    return None


#======================================================
__instance = None
def Init(propFile):
  global __instance
  __instance = Prop(propFile)


def GetInstance():
  return __instance