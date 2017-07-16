#include <iostream>
#include <stdio.h>
#include <string>
#include <math.h>
#include <fstream>
#include <sstream>
#include <stack>
#include "matrix.h"
#define VEC_LEN 100000
#define MAX_FC 1000
#define int_max 999
#include <stack>
using namespace std;

/**�ֵ���ö�̬���䷽ʽ����*/
struct DICOS
{
    string dic[VEC_LEN];
    double arg[VEC_LEN];
    int len;

};

/**
����ֻ�Ǽ򵥵Ĳ��ԣ�����δ���ö�̬����ķ�ʽ������ִʽṹ��
*/
struct FenCi
{
    string fenci[MAX_FC];//�ִʵĸ���
    double adj[MAX_FC][MAX_FC];//�ڽӾ���0��ʾ�������ӣ������ȡ������ȡ����Ϊ����Ȩ�أ�����ʹ�����·���㷨
    int pos[MAX_FC];//��¼�����ֵ��е�λ�ã������ȡ�ʵ�һԪ����
    int len;//�ִʵ�ʵ�ʳ���
};
FenCi fc;
bool visited[MAX_FC];//���·�����ʱ�־

DICOS dicos;

void createVocabList(dataToMatrix dtm)
{
    ofstream ofile_arg;
    ofstream ofile;
    ofile.open("data\\N_gram_dic.txt");
    int i,j,k,vl;
    int dic_len=0;
    Data *p;
    p=dtm.dataSet->next;
    for(i=0; i<dtm.col; i++)
    {
        for(j=0; p->attr_string[j]!="#"; j++)
        {
            for(vl=0; vl<dic_len; vl++)
            {
                if(!p->attr_string[j].compare(dicos.dic[vl]))
                {
                    dicos.arg[vl]++;//�Դ�Ƶ����ͳ�ƣ���Ϊ���ʲ���
                    break;
                }
            }
            if(vl==dic_len)
            {
                dicos.dic[vl]=p->attr_string[j];//���ֵ������չ
                dicos.arg[vl]=1;
                ofile<<p->attr_string[j]<<" ";
                dic_len++;
            }
        }
        p=p->next;
    }
    ofile.close();
    dicos.len=dic_len;
    ofile_arg.open("data\\N_gram_arg.txt");
    for(i=0; i<dicos.len; i++)
        ofile_arg<<dicos.arg[i]<<" ";
    ofile_arg.close();
    cout<<"vec_len="<<dic_len<<endl;
}

int Ngram(dataToMatrix testDtm)
{
    int i,j,k;
    Data *p;
    p=testDtm.dataSet->next;
    double maxp=1;
    for(i=0; testDtm.col&&p!=NULL; i++)
    {
        j=0;
        fc.fenci[j++]="BOS";//������ӳ�ʼ�ڵ�
        for(k=0; k<dicos.len; k++)
        {
            if(p->attr_string[0].find(dicos.dic[k],0)!=string::npos)
            {
                for(i=0; i<p->attr_string[0].length(); i+=2)
                {
                    if(p->attr_string[0].substr(i,2)==dicos.dic[k].substr(0,2))
                    {//���ǵ����ĵı������⣬��ƥ����ַ������ٴ�ȷ���Ƿ���Ϸִ�Ҫ��
                        //cout<<dicos.dic[k]<<"  ";
                        fc.fenci[j]=dicos.dic[k];
                        fc.pos[j]=k;
                        //cout<<"k="<<k<<"fc="<<fc.fenci[j]<<"pinfo="<<fc.pinfo[j]<<endl;
                        j++;
                    }
                }

            }
        }
        fc.fenci[j++]="END";//������ӵ���ֹ�ڵ�
        fc.len=j;
        p=p->next;
    }
    for(i=0; i<fc.len; i++)
    {
        //cout<<fc.fenci[i]<<"    ";
    }
    //cout<<endl;
    p=testDtm.dataSet->next;
    string sen="";
    sen.append("BOS");
    sen.append(p->attr_string[0]);
    sen.append("END");
    ofstream onfile;
    onfile.open("data\\pp.txt");
    for(i=0; i<fc.len; i++)
    {
        for(j=0; j<fc.len; j++)
        {
            fc.adj[i][j]=-1;
            if(sen.find(fc.fenci[i]+fc.fenci[j],0)!=string::npos)
            {
                fc.adj[i][j]=-log((dicos.arg[int(fc.pos[j])])/dicos.len);
                //�������Ȼ����ת��Ϊ���·���Ĺؼ��任
            }
            cout<<fc.adj[i][j]<<"  ";
            onfile<<fc.adj[i][j];
        }
        cout<<endl;
    }
    return 0;
}

void showpath(double *path,int v,int v0)
{
    stack<int> s;
    int u=v;
    while(v!=v0)
    {
        s.push(v);
        v=path[v];
    }
    s.push(v);
    while(!s.empty())
    {
        cout<<fc.fenci[s.top()]<<'/';
        s.pop();
    }
}

void shortpath_dij(int v0)
{
    int i,j,k;
    double *path,*dist;
    path=(double  *)malloc(sizeof(double)*MAX_FC);
    dist=(double *)malloc(sizeof(double)*MAX_FC);
    for(i=0; i<fc.len; i++)
    {
        if(fc.adj[v0][i]>0&&i!=v0)//��ʼ��
        {
            dist[i]=fc.adj[v0][i];//��¼�´���㣨��һ�����·���ϵĵ㣩
            //������ľ���
            path[i]=v0;//path��¼���·���ϴ�v0��vi��ǰһ�����㣬���������
            //���·�������������õ���һ�ַ��صķ�ʽ���б���
        }
        else
        {
            dist[i]=int_max;
            path[i]=v0;//��v0�����ڵĶ���Ĭ���������·���ϵ�ǰһ�������v0
        }
        visited[i]=false;
    }
    path[v0]=v0;
    dist[v0]=0;
    visited[v0]=true;
    for(i=1; i<fc.len; i++)
    {
        int min=int_max;
        int u=v0;
        for(j=0; j<fc.len; j++)
        {
            if(visited[j]==false&&dist[j]<min)
            {
                min=dist[j];
                u=j;
            }
        }
        visited[u]=true;//��ʵ��������ʾ�Ƿ��Ѿ������·���ϣ�����ڵĻ�����true��
        //����Ͳ���������Ѱ�����·��
        for(k=0; k<fc.len; k++)
        {
            if(visited[k]==false&&fc.adj[u][k]>0&&min+fc.adj[u][k]<dist[k])
            {
                dist[k]=min+fc.adj[u][k];
                path[k]=u;
            }
        }
    }
    for(i=0;i<fc.len;i++)
    {
        showpath(path,i,v0);
        cout<<":  "<<v0<<"->"<<i<<"="<<dist[i]<<endl;

    }
    int request=fc.len-1;
    showpath(path,request,v0);//������ѯ�����·�����
    //cout<<dist[request]<<endl;

}

int main()
{
    int k,i=0,j=0;
    //char file[20]="ddata\\ic.txt";
    ifstream infile;
    infile.open("data\\dicfile.txt",ios::in);
    string tmpstrline;
    while(!infile.eof())
    {
        getline(infile,tmpstrline,'\n');//��ȡ�ļ���һ�е����ݣ�����Ϊstring����
        stringstream input(tmpstrline);
        if(tmpstrline!="\0")////���ڶ�ȡ�ļ�������ͬ��������ò���
        {
            while(input>>dicos.dic[j])
            {
                j++;
            }
        }
    }
    dicos.len=j;
    infile.close();
    i=0;
    infile.open("data\\N_gram_arg.txt");
    while(!infile.eof())
    {
        getline(infile,tmpstrline,'\n');//��ȡ�ļ���һ�е����ݣ�����Ϊstring����
        stringstream input(tmpstrline);
        if(tmpstrline!="\0")////���ڶ�ȡ�ļ�������ͬ��������ò���
        {
            while(input>>dicos.arg[i])
            {
                i++;
            }
        }
    }
    dicos.len=i;
    double sum=0;
    for(i=0; i<dicos.len; i++)
    {
        sum+=dicos.arg[i];
    }
    cout<<"�ܵ�������"<<sum<<"�ֵ䳤�ȣ�"<<dicos.len<<endl;

    dataToMatrix testDtm;
    testDtm.loadData(&testDtm,"data\\Ntest.txt");
    testDtm.print(testDtm);
    Ngram(testDtm);
    shortpath_dij(0);

}

