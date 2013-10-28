import MySQLdb
import json
from bottle import route, run, request
import warnings
import urllib

_MULTLEVELCOUNT = 1
_SINGLELEVELCOUNT = 1

db = MySQLdb.connect(host="localhost", # your host, usually localhost
                     user="james", # your username
                      passwd="password", # your password
                      db="testdb") # name of the data base
					  
@route('/searchForSession', method='POST')
def searchForSession():
	warnings.filterwarnings('ignore')
	data = request.body.readline()
	
	cur = db.cursor()
	cur.execute('CREATE TABLE IF NOT EXISTS `session`(ip VARCHAR(32), port VARCHAR(32))')
	cur.execute("INSERT INTO session(ip, port) VALUES('"+request.remote_addr+"', '"+data+"')")
	db.commit()
	
	cur.execute("SELECT * FROM session")
	rows = cur.fetchall()
	xmlString = '<document>'

	for row in rows:
		xmlString = xmlString+'<player><ip>'+row[0]+'</ip><port>'+row[1]+'</port></player>'
		
	xmlString = xmlString + '</document>'
	return xmlString

@route('/getSession', method='GET')
def getSession():

	cur = db.cursor()
	cur.execute("SELECT * FROM session")
	rows = cur.fetchall()
	xmlString = '<document>'

	for row in rows:
		xmlString = xmlString+'<player><ip>'+row[0]+'</ip><port>'+row[1]+'</port></player>'
		
	xmlString = xmlString + '</document>'
	return xmlString

@route('/destroySession', method='GET')
def destroySession():

	cur = db.cursor()
	cur.execute("DROP TABLE session")
	
@route('/sendScoreAuth', method='POST')
def sendScoreAuth():

        data = request.body.readline()
        print "Add score: "
        print data

        cur = db.cursor()
        token, newScore, levelName = data.split(",")
        userId = ""

        try:
                cur.execute( "SELECT idNum FROM authusers WHERE token = '"+token+"'")
                userId = cur.fetchall()

                print 'valid'
                
                cur.execute( "SELECT score FROM "+levelName+"scores WHERE idNum = '" + userId[0][0] +"' ")
                oldScore = cur.fetchall()

                print 'Got old score'
                
                if int(oldScore[0][0]) < int(newScore):
                        cur.execute("update "+levelName+"scores set score="+str(newScore)+" where idNum = '"+userId[0][0]+"'")
                
                db.commit()
                        
        except:
                print 'token not in user table'
                
                
@route('/getScores/:id', method='GET')
def getScores(id):

        cur = db.cursor()
        cur.execute( "SELECT score FROM tutorialscores WHERE idNum = '" + id +"' ")

        levelName = "tutorial"
        score = cur.fetchall()
        xmlString = '<document>'
        xmlString = xmlString+'<scoreInfo><levelName>'+levelName+'</levelName><score>'+str(score[0][0])+'</score></scoreInfo>'

        for x in range(1, _MULTLEVELCOUNT+1):
                cur.execute( "SELECT score FROM multlevel"+str(x)+"scores WHERE idNum = '" + id +"' ")
                levelName = "multlevel"+str(x)
                score = cur.fetchall()
                xmlString = xmlString+'<scoreInfo><levelName>'+levelName+'</levelName><score>'+str(score[0][0])+'</score></scoreInfo>'

        for x in range(1, _SINGLELEVELCOUNT+1):
                cur.execute( "SELECT score FROM level"+str(x)+"scores WHERE idNum = '" + id +"' ")
                levelName = "level"+str(x)
                score = cur.fetchall()
                xmlString = xmlString+'<scoreInfo><levelName>'+levelName+'</levelName><score>'+str(score[0][0])+'</score></scoreInfo>'

        xmlString = xmlString + '</document>'
        return xmlString

@route('/createUserAuth', method='POST')
def createUserAuth():

	data = request.body.readline()
	print "Create auth user: "
	print data

	requestID, token = data.split(",")

        # Athenticate token with facebook
	url = 'https://graph.facebook.com/me?access_token='+token
	fp = urllib.urlopen(url)
	responseString = fp.read()
	jsonData = json.loads(responseString)

	# Check response
	try:
		responseID = jsonData['id']
		if responseID == requestID:
			print 'valid'
			cur = db.cursor()
			cur.execute( "INSERT INTO authusers(idNum, token) SELECT * FROM(SELECT '"+requestID+"','"+token+"') AS tmp WHERE NOT EXISTS( SELECT idNum FROM authusers WHERE idNum = '"+responseID+"' ) LIMIT 1" )
			cur.execute( "INSERT INTO tutorialscores(idNum, score) SELECT * FROM(SELECT '"+responseID+"', 0) AS tmp WHERE NOT EXISTS( SELECT idNum FROM tutorialScores WHERE idNum = '"+ responseID +"' ) LIMIT 1" )

		for x in range(1, _MULTLEVELCOUNT+1):
			table = "multlevel" + str(x) + "scores"
			cur.execute( "INSERT INTO "+table+"(idNum, score) SELECT * FROM(SELECT '"+responseID+"', 0) AS tmp WHERE NOT EXISTS( SELECT idNum FROM "+table+" WHERE idNum = '"+ responseID +"' ) LIMIT 1" )

		for x in range(1, _SINGLELEVELCOUNT+1):
			table = "level" + str(x) + "scores"
			cur.execute( "INSERT INTO "+table+"(idNum, score) SELECT * FROM(SELECT '"+responseID+"', 0) AS tmp WHERE NOT EXISTS( SELECT idNum FROM "+table+" WHERE idNum = '"+ responseID +"' ) LIMIT 1" )

		db.commit()

	except ValueError: # Not a valid token
		print 'Not a valid token'

@route('/deleteUserAuth', method='POST')
def deleteUserAuth():

        data = request.body.readline()
        print "Create auth user: "
        print data

        requestID, token = data.split(",")

        url = 'https://graph.facebook.com/me?access_token='+token
        fp = urllib.urlopen(url)
        responseString = fp.read()
        jsonData = json.loads(responseString)

        try:
                responseID = jsonData['id']
                if responseID == requestID:

                        cur = db.cursor()
                        cur.execute( "DELETE FROM authusers WHERE idNum = '"+responseID+"'" )
                        db.commit()

        except ValueError: # Not a valid token
                print 'Not a valid token'


@route('/leaveSession', method='POST')
def leaveSession():

        data = request.body.readline()
        print "Leave session: "
        print data

        requestID = data

        try:
                cur = db.cursor()

                cur.execute( "DELETE FROM session WHERE idNum = '"+requestID+"'" )
                    
                db.commit()

        except ValueError: # Not a valid token
                print 'Not a valid user'
                

run(host='10.40.3.237', port=28003)
