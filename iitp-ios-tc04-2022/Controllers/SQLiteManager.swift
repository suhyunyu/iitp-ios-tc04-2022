//
//  SqliteManager.swift
//  iitp-ios-tc04-2022
//
//  Created by lfin on 2022/04/26.
//

import Foundation
import SQLite3

class SQLiteManager{
    static let shared = SQLiteManager()
    
    //db를 가리키는 포인터
    var db : OpaquePointer?
    
    
    
    // db
    let databaseName = "ttadb.sqlite"
    
    init() {
        self.db = createDB()
    }

    deinit {
        sqlite3_close(db)
    }
    
    private func createDB() -> OpaquePointer? {
        var db: OpaquePointer? = nil
        do {
            let dbPath: String = try FileManager.default.url(for: .documentDirectory, in: .userDomainMask, appropriateFor: nil, create: false).appendingPathComponent(databaseName).path
            
            if sqlite3_open(dbPath, &db) == SQLITE_OK {
                print("Successfully created DB. Path: \(dbPath)")
                return db
            }
        } catch {
            print("Error while creating Database -\(error.localizedDescription)")
        }
        return nil
    }
    
    //테이블 생성
    func createInfoTable(){
        
        let query = """
           CREATE TABLE IF NOT EXISTS TtaInfo (b_file_name TEXT, q_file_name TEXT,
           metadata TEXT);
           """
        
        var statement: OpaquePointer? = nil
        
        if sqlite3_prepare_v2(self.db, query, -1, &statement, nil) == SQLITE_OK {
            if sqlite3_step(statement) == SQLITE_DONE {
                print("Creating table has been succesfully done. db: \(String(describing: self.db))")
                
            }
            else {
                let errorMessage = String(cString: sqlite3_errmsg(db))
                print("\nsqlte3_step failure while creating table: \(errorMessage)")
            }
        }
        else {
            let errorMessage = String(cString: sqlite3_errmsg(self.db))
            print("\nsqlite3_prepare failure while creating table: \(errorMessage)")
        }
        
        sqlite3_finalize(statement) // 메모리에서 sqlite3 할당 해제.
    }
    
    
    // 데이터 정보 삽입
    func insertData(tc04: TC04Base64){
        // 명령 객체 만들기
        var statement: OpaquePointer? = nil
        
        // 쿼리문
        let insertQuery = "insert into TtaInfo (b_file_name, q_file_name, metadata) values (?, ?, ?);"
        
        if sqlite3_prepare_v2(self.db, insertQuery, -1, &statement, nil) == SQLITE_OK {
            
            sqlite3_bind_text(statement, 1, NSString(string: tc04.bFileName).utf8String , -1, nil)
            sqlite3_bind_text(statement, 2, NSString(string: tc04.qFileName).utf8String, -1, nil)
            sqlite3_bind_text(statement, 3, NSString(string: tc04.metadata).utf8String, -1, nil)
            
        }
        else {
            print("sqlite binding failure")
        }
        
        if sqlite3_step(statement) == SQLITE_DONE {
            print("sqlite insertion success")
        }
        else {
            let errMsg = String(cString : sqlite3_errmsg(db)!)
            print("sqlite step failure :: ",errMsg)
        }
    }
    
    // 데이터 정보 조회
    func readData() -> [TC04Base64] {
        let query: String = "select b_file_name, q_file_name, metadata from TtaInfo;"
        var statement: OpaquePointer? = nil
        // 아래는 [MyModel]? 이 되면 값이 안 들어간다.
        // Nil을 인식하지 못하는 것으로..
        var result: [TC04Base64] = []

        if sqlite3_prepare(self.db, query, -1, &statement, nil) != SQLITE_OK {
            let errorMessage = String(cString: sqlite3_errmsg(db)!)
            print("error while prepare: \(errorMessage)")
            return result
        }
        while sqlite3_step(statement) == SQLITE_ROW {
            
            let bFileName = String(cString: sqlite3_column_text(statement, 0))
            let qFileName =  String(cString: sqlite3_column_text(statement, 1))
            let metadata =  String(cString: sqlite3_column_text(statement, 2))

            
            result.append(TC04Base64(bFileName: String(bFileName), qFileName: String(qFileName), metadata: String(metadata)))
            
        }
        sqlite3_finalize(statement)
        
        return result
    }
    
    //데이터 정보 삭제
    func deleteInfoAll(){
        let DELETE_QUERY = "DELETE FROM TtaInfo"
        var stmt:OpaquePointer?
        
        print(DELETE_QUERY)
        if sqlite3_prepare_v2(db, DELETE_QUERY, -1, &stmt, nil) != SQLITE_OK{
            let errMsg = String(cString: sqlite3_errmsg(db)!)
            print("error preparing delete: v1\(errMsg)")
            return
        }
        
        if sqlite3_step(stmt) != SQLITE_DONE {
            let errMsg = String(cString : sqlite3_errmsg(db)!)
            print("delete fail :: \(errMsg)")
            return
        }
        sqlite3_finalize(stmt)

    }
    
    
}
