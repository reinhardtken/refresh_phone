#!/usr/bin/env python
# -*- coding: utf-8 -*-
import MySQLdb
import time
import datetime
import Queue
#====================================
import util.utility



def Connect(user_param, password, db):
  return MySQLdb.connect(host='localhost', user=user_param, passwd=password, db=db, port=3306, charset='utf8')


def CreateTable(con, statement):
  #table_name = DepthMQMySqlProxy.TableName(self.year_month, type)
  #statement = DepthMQMySqlProxy.CREATE_BASIC_TABLE % (table_name)
  #print(statement)
  cur = con.cursor()
  try:
    cur.execute(statement)
    con.commit()
  except MySQLdb.Error, e:
    print ("CreateTable Error %d: %s" % (e.args[0], e.args[1]))
  cur.close()

def CheckTableExist(con, db, tablename):
  check_statement = ("SELECT table_name "
                      "FROM information_schema.tables "
                      "WHERE table_schema = '" + db + "' "
                      "AND table_name = %s;")
  re = ExecuteStatement(con, check_statement, [tablename])
  if re > 0:
    return True
  else:
    return False




def ExecuteStatement(con, statement, params):
  cur = con.cursor()
  re = 0
  #print(statement)
  try:
    #cur.execute(statement, info)
    re = cur.execute(statement, params)
    con.commit()
  except MySQLdb.Error, e:
    print ("ExecuteStatement Error %d: %s" % (e.args[0], e.args[1]))

  cur.close()
  return re



def ExecuteSQLFile(con, file):
  statement = open(file).read()
  statements = statement.split(";")
  for v in statements:
    v = v + ";"
    #print(v)
    ExecuteStatement(con, v, [])