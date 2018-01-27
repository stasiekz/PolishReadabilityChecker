import json
import urllib2
import glob
import os
import time

user = "mojadresemai"
lpmn = "any2txt|wcrft2({\"morfeusz2\":false})"
url = "http://ws.clarin-pl.eu/nlprest2/base"


def upload(file):
    with open(file, "rb") as myfile:
        doc = myfile.read()
    return urllib2.urlopen(urllib2.Request(url + '/upload/', doc, {'Content-Type': 'binary/octet-stream'})).read()


def process(data):
    doc = json.dumps(data)
    taskid = urllib2.urlopen(urllib2.Request(url + '/startTask/', doc, {'Content-Type': 'application/json'})).read()
    time.sleep(0.2)
    resp = urllib2.urlopen(urllib2.Request(url + '/getStatus/' + taskid))
    data = json.load(resp)
    while data["status"] == "QUEUE" or data["status"] == "PROCESSING":
        time.sleep(0.5)
        resp = urllib2.urlopen(urllib2.Request(url + '/getStatus/' + taskid))
        data = json.load(resp)
    if data["status"] == "ERROR":
        print("Error " + data["value"])
        return None
    return data["value"]


def main():
    in_path = 'files/paczka/*'
    out_path = 'files/out/'
    global_time = time.time()
    for file in glob.glob(in_path):
        start_time = time.time()
        fileid = upload(file)
        print("Processing: " + file)
        data = {'lpmn': lpmn, 'user': user, 'file': fileid}
        data = process(data)
        if data == None:
            continue
        data = data[0]["fileID"]
        content = urllib2.urlopen(urllib2.Request(url + '/download' + data)).read()
        with open(out_path + os.path.basename(file) + '.ccl', "w") as outfile:
            outfile.write(content)
    print("GLOBAL %s seconds ---" % (time.time() - global_time))


main()