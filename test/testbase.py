import unittest
import httplib
import ConfigParser
import re
import json

class MQWebTest(unittest.TestCase):

	headers = {
		'Accept': 'application/json',
		'Content-Type': 'application/json; charset=UTF-8'
	}

	def setUp(self):
		config = ConfigParser.ConfigParser()
		config.add_section('MQWeb')
		config.set('MQWeb', 'host', 'localhost')
		config.set('MQWeb', 'port', '8081') 
		config.read('test.cfg')
		self.qmgr = config.get('MQ', 'qmgr')
		self.mqWebHost = config.get('MQWeb', 'host')
		self.mqWebPort = config.get('MQWeb', 'port')

		print
		print 'Running test with the following configuration:'
		print 'MQWeb Host: ' + self.mqWebHost
		print 'MQWeb Port: ' + self.mqWebPort
		print 'MQ QueueManager: ' + self.qmgr
		print

	def checkIds(self, data):
		ids = []
		for key in data:
			m = re.search(r"id_([0-9]+)", key)
			if m :
				ids.append(int(m.group(1)))

		if len(ids) > 0:
			print json.dumps(data, indent=4)
			print str(ids)
			return False
			
		return True

	def getJSON(self, url):
		print 'Trying to connect to ' + url
		try:
			conn = httplib.HTTPConnection(self.mqWebHost, self.mqWebPort)
			conn.request('GET', url, "", MQWebTest.headers)
			res = conn.getresponse()
			data = json.loads(res.read())
		except httplib.HTTPException as e:
			print 'Exception Caught: ' + e.errno + e.strerror
			self.assertFalse(True, "Can't connect to MQWeb: " + self.mqWebHost + ":" + self.mqWebPort + ' (qmgr: ' + self.qmgr + ')')

		return data
		
	def appJSON(self, url, filter):
		print 'Trying to connect to ' + url
		try:
			conn = httplib.HTTPConnection(self.mqWebHost, self.mqWebPort)
			conn.request('POST', url, json.dumps(filter), MQWebTest.headers)
			res = conn.getresponse()
			data = json.loads(res.read())
		except httplib.HTTPException as e:
			print 'Exception Caught: ' + e.errno + e.strerror
			self.assertFalse(True, "Can't connect to MQWeb: " + self.mqWebHost + ":" + self.mqWebPort + ' (qmgr: ' + self.qmgr + ')')

		return data