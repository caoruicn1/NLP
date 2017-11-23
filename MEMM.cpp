/**

MEMM��MEģ����MMģ�ͣ�����Ʒ�ģ�ͣ�ȥ�������ĸ���Ľ�ϣ����õ�˼���������ģ�͵�˼�룬��ʹ��������Լ�������������
�����ż��ʽ���Կ����ǣ����p��y|x��=wf(x,y)/Z(x)������f(x,y)�����������Ķ��壬wΪ�������Ĳ�����
Z(x)��y��x�������µ����п�ȡֵ���ʺͣ�����һ����������Ǿֲ���һ�����ᵼ�±��ƫ�á�

��������ʽ����MM��ʽ��������״̬��۲�ֵ��״̬�ȿ������ֹ��ʵ�BMES��Ҳ�����Ǵ��Ա�׼����ʽ

��ζ�������������Ĺؼ�����������ϲ���p(Si|oi,si-1)����ʽ���������������ǰ��״̬�͵�ǰ�۲�ֵ��
����������p(Si,oi,si-1)=p(Si|oi,si-1)=p(Si|oi)p(Si|si-1)��ͼ򻯲��������ϸ���д������������ʽ
��һ���Ϊ��oi��si-1����������siȡֵ�ĸ��ʺ�

�������Ʒ������ݶȷ���ţ�ٷ���GIS
��Ȼ��������ؼ�����Ȼ���ƣ��������ģ�͵�˼�룬��������£����������������ʣ��������ϸ��ʵ�����£������͵���ͳ�Ƴ������������ʡ�
��������ǲ����������ϸ�����Ϊ�������������²�����������������ǣ�


�������ͬ���ǲ���Viterbi�㷨�������ǲ��ù�һ����ʹ�������󻯣���һ���ǽ����������Ļ����ϣ�
�����һ�����ǹ�һ��p��y|x��=wf(x,y)/Z(x)����ô���������·������Ҫ��Viterbi�㷨�����ϸã�������û�н��й�һ��
ֻ����HMMģ���е���Щ��ͬ


**/

#include <iostream>
#include <stdio.h>
#include <string>
#include <math.h>
#include <fstream>
#include <sstream>
#include <stack>
#include <map>
#include "matrix.h"
#define VEC_LEN 5000
#define STATE 4
#define MAX_FC 20000
#define int_max 999
#define double_min -1
#include <stack>
using namespace std;

struct DICOS
{
    string dic[VEC_LEN];//�۲�ֵ�ֵ�
    double pos[VEC_LEN][STATE];//����p(si|oi)
    double pss[STATE][STATE];//����p(si|si-1)
    double pos0[STATE];//����ʹ��p(s1|o1,s0)=p(s1|o1)*p(s1)=p(s|o)*p(s1)
    int len;
};
DICOS dicos;

/**

�����ֵ��ò��������й۲����ڵ�λ��

**/

int getPos(string str)
{
    int i=0;
    for(i=0; i<dicos.len; i++)
    {
        if(!str.compare(dicos.dic[i]))
            return i;
    }
    cout<<"״̬�ֵ��в����ڸ�״̬"<<endl;
    return -1;
}

/**

    ����״̬�����������±�

**/

int getState(string str)
{
    if(str=="0")
        return 0;
    if(str=="1")
        return 1;
    if(str=="2")
        return 2;
    if(str=="3")
        return 3;
    return 0;
}
/**

    ���ڴ���λ�ö�Ӧ��״̬ӳ�����

**/
int wordToState(Data *p,int word,int j,int vl,string &state)
{
    char ch;
    if(p->attr_string[j].length()==2)
    {
        dicos.pos[vl][3]++;
        state.append(1,'3');
        state.append(1,' ');
    }
    if(p->attr_string[j].length()==4)
    {
        if(word==0)
        {
            dicos.pos[vl][0]++;
            state.append(1,'0');
            state.append(1,' ');
        }
        else
        {
            dicos.pos[vl][2]++;
            state.append(1,'2');
            state.append(1,' ');
        }
    }
    if(p->attr_string[j].length()==6)
    {
        dicos.pos[vl][word/2]++;
        ch=(char)(word/2+48);
        state.append(1,ch);
        state.append(1,' ');
    }
    if(p->attr_string[j].length()==8)
    {
        if(word==0)
        {
            dicos.pos[vl][0]++;
            state.append(1,'0');
            state.append(1,' ');
        }
        if(word==6)
        {
            dicos.pos[vl][2]++;
            state.append(1,'2');
            state.append(1,' ');
        }
        else
        {
            dicos.pos[vl][1]++;
            state.append(1,'1');
            state.append(1,' ');
        }
    }
}

/**

    ����ѵ�����ݣ���ȡ�۲�ֵ�ֵ䣬���ѹ۲��ֵ�д���ļ��У�
    ͬʱ���Եõ�HMMģ���е�������Ҫ����

**/

void createVocabList(dataToMatrix dtm)
{
    ofstream ofile;
    ofstream ofile_arg;
    ofstream ofile_state;
    ofile.open("data\\memm_dic.txt");
    ofile_state.open("data\\memm_state.txt");
    int i,j,k,vl;
    int dic_len=0;
    int word;
    Data *p;
    p=dtm.dataSet->next;
    string state;
    char ch;
    double sum;
    /**
    �ֵ佨��
    */
    for(i=0; i<dtm.col&&p!=NULL; i++)
    {
        state="";
        for(j=0; p->attr_string[j]!="#"&&j<p->len; j++)
        {
            if(p->attr_string[j].length()%2!=0||p->attr_string[j].length()>9)
                continue;
            for(word=0; word<p->attr_string[j].length(); word+=2)
            {
                for(vl=0; vl<dic_len; vl++)
                {
                    if(!p->attr_string[j].substr(word,2).compare(dicos.dic[vl]))
                    {
                        wordToState(p,word,j,vl,state);//p�Ǿ��ӣ�j�ǵڼ����ʣ�word�Ǵ��еĵڼ����֣�vl�������ֵ��е�λ��
                        break;
                    }
                }
                if(vl==dic_len)
                {
                    dicos.dic[vl]=p->attr_string[j].substr(word,2);//���ֵ������չ
                    wordToState(p,word,j,vl,state);
                    ofile<<p->attr_string[j].substr(word,2)<<"  ";
                    dic_len++;
                }
            }
        }
        state.append(1,'\n');
        p=p->next;
        ofile_state<<state;
    }
    ofile.close();
    ofile_state.close();
    dicos.len=dic_len;
    cout<<"col="<<dtm.col<<endl;
    cout<<"vec_len="<<dic_len<<endl;

    /**
    ��������
    **/

    ofile_arg.open("data\\memm_arg.txt");
    for(i=0; i<dicos.len; i++)
    {
        ofile_arg<<dicos.dic[i]<<"  ";
        sum=0;
        for(j=0; j<STATE; j++)
        {
            sum+=dicos.pos[i][j];
        }
        for(j=0; j<STATE; j++)
        {
            dicos.pos[i][j]/=sum;
            ofile_arg<<dicos.pos[i][j]<<"  ";
        }
        ofile_arg<<'\n';
    }
    ofile_arg.close();

    dataToMatrix state_dtm;
    state_dtm.loadData(&state_dtm,"data\\memm_state.txt");
    p=state_dtm.dataSet->next;
    for(i=0; i<state_dtm.col&&p!=NULL; i++)
    {
        dicos.pos0[getState(p->attr_string[0])]++;//ͳ�Ʋ���pos0
        for(j=1; p->attr_string[j]!="#"&&j<p->len&&p->len>1; j++)
        {
            dicos.pss[getState(p->attr_string[j-1])][getState(p->attr_string[j])]++;//ͳ�Ʋ���pss
        }
        p=p->next;
    }
    cout<<"i="<<i<<endl;
    cout<<"pss:--------------"<<endl;
    for(i=0; i<STATE; i++)
    {
        sum=0;
        for(j=0; j<STATE; j++)
        {
            sum+=dicos.pss[i][j];
        }
        for(j=0; j<STATE; j++)
        {
            dicos.pss[i][j]/=sum;
            cout<<dicos.pss[i][j]<<"  ";
        }
        cout<<endl;
    }
    cout<<"PI:--------------"<<endl;
    sum=0;
    for(i=0; i<STATE; i++)
    {
        sum+=dicos.pos0[i];
    }
    for(i=0; i<STATE; i++)
    {
        dicos.pos0[i]/=sum;
        cout<<dicos.pos0[i]<<"  ";
    }
    cout<<endl;

}

int Viterbi(dataToMatrix testDtm)
{
    int t,i,j,k;
    int pos;
    Data *p;
    p=testDtm.dataSet->next;
    double deta[VEC_LEN][STATE];
    int fai[VEC_LEN][STATE];
    double max_deta;
    double max_fai;
    int max_i;
    for(i=0; i<STATE; i++)
    {
        pos=getPos(p->attr_string[0]);
        deta[0][i]=dicos.pos0[i]*dicos.pos[pos][i];
        fai[0][i]=0;
    }
    for(i=0; i<testDtm.col&&p!=NULL; i++)
    {
        for(t=1; t<p->len; t++)
        {
            for(i=0; i<STATE; i++)
            {
                max_deta=double_min;
                max_fai=double_min;
                for(j=0; j<STATE; j++)
                {
                    pos=getPos(p->attr_string[t]);
                    if(deta[t-1][j]*dicos.pss[j][i]*dicos.pos[pos][i]>max_deta)
                    {
                        max_deta=deta[t-1][j]*dicos.pss[j][i]*dicos.pos[pos][i];
                    }
                    if(deta[t-1][j]*dicos.pss[j][i]>max_fai)
                    {
                        max_fai=deta[t-1][j]*dicos.pss[j][i];
                        max_i=j;
                    }
                }
                deta[t][i]=max_deta;
                fai[t][i]=max_i;
            }
        }
        max_deta=double_min;
        for(i=0; i<STATE; i++)
        {
            if(deta[p->len-1][i]>max_deta)
            {
                max_deta=deta[p->len-1][i];
                max_i=i;
            }
            cout<<"P*="<<deta[p->len-1][i]<<endl;
        }
        cout<<max_i;
        for(t=p->len-2; t>=0; t--)
        {
            cout<<fai[t+1][max_i];
            max_i=fai[t+1][max_i];
        }
        p=p->next;
    }
}
int hmm(dataToMatrix testDtm)
{
    Viterbi(testDtm);
    return 0;
}

int main()
{
    dataToMatrix dtm;
    dtm.loadData(&dtm,"data\\pku_training.txt");//����ѵ����
    createVocabList(dtm);//����״̬�ֵ䣬���ڴ�Ų���ֵ
    dataToMatrix testDtm;//���ز�������
    testDtm.loadData(&testDtm,"data\\memm_test.txt");
    hmm(testDtm);//����HMM�㷨������testDtmΪ����״̬����
    return 0;
}
