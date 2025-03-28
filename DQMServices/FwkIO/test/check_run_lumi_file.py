from builtins import range
import ROOT as R
import sys

f = R.TFile.Open(sys.argv[1])

th1fs = f.Get("TH1Fs")

indices = f.Get("Indices")

expectedIndices = list()
values = list()
nRuns = 10
nHists = 10
nLumiPerRun = 1
startIndex = 0
lastIndex =-1
for i in range(0,nRuns):
    for l in range(0,1):
        for j in range(0,nHists):
            lastIndex +=1
            values.append(("Foo"+str(j)+"_lumi", 0, 1.0))
        expectedIndices.append( (i+1,l+1,3,startIndex,lastIndex) )
        startIndex = lastIndex+1
    for j in range(0,nHists):
        lastIndex +=1
        values.append(("Foo"+str(j), 0, 1.0))
    expectedIndices.append( (i+1,0,3,startIndex,lastIndex) )
    startIndex = lastIndex+1


if nRuns*nHists+nRuns*nLumiPerRun*nHists != th1fs.GetEntries():
    print("wrong number of entries in TH1Fs",th1fs.GetEntries())
    sys.exit(1)

if nRuns+nRuns*nLumiPerRun != indices.GetEntries():
    print("wrong number of entries in Indices", indices.GetEntries())
    sys.exit(1)

indexTreeIndex = 0
for run in range(0,nRuns):
    for lumi in range(0,1):
        indices.GetEntry(indexTreeIndex)
        v = (indices.Run,indices.Lumi,indices.Type,indices.FirstIndex,indices.LastIndex)
        if v != expectedIndices[indexTreeIndex]:
            print('ERROR: unexpected value for indices at run,lumi :',run,lumi)
            print(' expected:', expectedIndices[indexTreeIndex])
            print(' found:',v)
            sys.exit(1)
        for ihist in range(indices.FirstIndex,indices.LastIndex+1):
            index = ihist
            th1fs.GetEntry(ihist)
            v = (th1fs.FullName,th1fs.Flags,th1fs.Value.GetEntries())
            if v != values[index]:
                print('ERROR: unexpected value for index, runIndex,lumiIndex :',index,run,lumi)
                print(' expected:',values[index])
                print(' found:',v)
                sys.exit(1)
        indexTreeIndex +=1
    indices.GetEntry(indexTreeIndex)
    for ihist in range(indices.FirstIndex,indices.LastIndex+1):
        index = ihist
        th1fs.GetEntry(ihist)
        v = (th1fs.FullName,th1fs.Flags,th1fs.Value.GetEntries())
        if v != values[index]:
            print('ERROR: unexpected value for index, runIndex :',index,run)
            print(' expected:',values[index])
            print(' found:',v)
            sys.exit(1)
    indexTreeIndex +=1

print("SUCCEEDED")

