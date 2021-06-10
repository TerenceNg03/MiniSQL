//
//  Catalog_Manager.cpp
//  MiniSQL
//
//  Created by Adam Wu on 2021/5/11.
//

#include "Catalog_Manager.hpp"
#include "Exception.hpp"

using namespace std;
#include <string.h>

const string Catalog_Manager::catalog_filename = "DB_Data/catalog";


Catalog_Manager::Catalog_Manager(){
    load_catalog();
}

Catalog_Manager::~Catalog_Manager(){
    dump_catalog();
}

void Catalog_Manager::insert(const db_table& tab){
    if(find_if(catalogs.begin(), catalogs.end(), [=](auto dbt)->bool{return (dbt.name==tab.name);})!=catalogs.end()){
        throw FormatException("Can not recreate existing table.\n");
    }
    catalogs.push_back(tab);
    string s ="> "+tab.filename;
    system(s.c_str());
}
void Catalog_Manager::erase(const std::string& name){
    auto it = find_if(catalogs.begin(), catalogs.end(), [=](auto dbt)->bool{return (dbt.name==name);});
    if(it!=catalogs.end()){
        string s = "rm "+it->filename;
        system(s.c_str());
        catalogs.erase(it);
    }else{
        throw FormatException("Can not drop non-exist table %s.\n",name.c_str());
    }
    
}

/*
 format
 
 256 name
 256 filename
 size
 coloums_number

 coloums:
    type
    256 char name
    bool IsUnique
    bool IsPrimary
    (optional) length
 */

void Catalog_Manager::dump_catalog(){
    FILE* fp = fopen(catalog_filename.c_str(), "wb");
    char s[256] = {0};
    int cat_size = (int)catalogs.size();
    fwrite(&cat_size, sizeof(cat_size), 1, fp);
    for(auto t : catalogs){
        strcpy(s,t.name.c_str());
        fwrite(s, sizeof(char), 256, fp);
        memset(s, 0, sizeof(s));
        strcpy(s, t.filename.c_str());
        fwrite(s, sizeof(char), 256, fp);;
        memset(s, 0, sizeof(s));
        fwrite(&t.size, sizeof(t.size), 1, fp);
        int n_cols = (int)t.columns.size();
        fwrite(&n_cols, sizeof(n_cols), 1, fp);
        for(auto col: t.columns){
            fwrite(&col.T, sizeof(col.T), 1, fp);
            memset(s, 0, sizeof(s));
            strcpy(s, col.name.c_str());
            fwrite(s, sizeof(char), 256, fp);
            fwrite(&col.IsUnique, sizeof(col.IsUnique), 1, fp);
            fwrite(&col.IsPrimary, sizeof(col.IsPrimary), 1, fp);
            if(col.T==db_item::type::DB_CHAR)fwrite(&col.length, sizeof(col.length), 1, fp);
        }
    }
    fclose(fp);
}

void Catalog_Manager::load_catalog(){
    catalogs.clear();
    system("[ -d DB_Data ] || mkdir DB_Data");
    FILE* fp = fopen(catalog_filename.c_str(), "rb");
    if(fp==NULL)return;
    int cata_size;
    fread(&cata_size, sizeof(int), 1, fp);
    //printf("cata_size : %d\n",cata_size);
    for(int i=0; i<cata_size; i++){
        char s[256];
        db_table dbt;
        fread(s, sizeof(char), 256, fp);
        dbt.name = s;
        fread(s, sizeof(char), 256, fp);
        dbt.filename = s;
        fread(&dbt.size, sizeof(dbt.size), 1, fp);
        int n_cols;
        fread(&n_cols, sizeof(n_cols), 1, fp);
        //printf("n_cols : %d\n",n_cols);
        for(int i=0; i<n_cols; i++){
            db_table::column col;
            fread(&col.T, sizeof(col.T), 1, fp);
            fread(s, sizeof(char), 256, fp);
            col.name = s;
            fread(&col.IsUnique, sizeof(col.IsUnique), 1, fp);
            fread(&col.IsPrimary, sizeof(col.IsPrimary), 1, fp);
            if(col.T==db_item::type::DB_CHAR)fread(&col.length, sizeof(col.length), 1, fp);
            dbt.columns.push_back(col);
        }
        catalogs.push_back(dbt);
    }
};

void Catalog_Manager::clear(){
    FILE* fp = fopen(catalog_filename.c_str(), "w");
    int cat_size = 0;
    fwrite(&cat_size, sizeof(cat_size), 1, fp);
    fclose(fp);
    catalogs.clear();
}

void catalog_unit_test::test(){
    Catalog_Manager* ctm = new Catalog_Manager();
    ctm->clear();
    db_table dbts[] = {
        {
            {
                {db_item::type::DB_INT,"ID",true,true},
                {db_item::type::DB_CHAR,"NAME",false,false,20}
            },
            "stu",10,"stu.dat"
        },
        {
            {
                {db_item::type::DB_FLOAT,"SALARY",false,false},
                {db_item::type::DB_CHAR,"NAME",true,false,20},
                {db_item::type::DB_INT,"ID",true,true}
            },
            "teacher",5,"teacher.dat"
        }
    };
    
    ctm->insert(dbts[0]);
    ctm->insert(dbts[1]);
    
    delete ctm;
    
    ctm = new Catalog_Manager();
    
}
