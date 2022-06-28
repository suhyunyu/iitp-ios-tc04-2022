//
//  TTADataManager.swift
//  iitp-ios-tc04-2022
//
//  Created by lfin on 2022/04/26.
//

import Foundation
import UIKit


class TTADataManager{
    
    
    @Published var posts: [TC04Base64] = []
    
    // 출력할 이미지 정보 json으로 받아오기
    func loadImageJsonData(fromURL url : URL ,completion: @escaping(_ data: TC04List?,_ error: Error?) -> Void){
        
        let Task = URLSession.shared.dataTask(with: url, completionHandler: { data, response, error in
            
            guard let data = data,error == nil,
                  let response = response as? HTTPURLResponse,
                  response.statusCode >= 200 && response.statusCode < 300
            else{
                completion(nil, error)
                return
            }
            
            do{
                
                let decoder = JSONDecoder()
                //decoder.keyDecodingStrategy = .convertFromSnakeCase
            
                guard let newPosts = try? decoder.decode(TC04List.self, from: data) else { return }
                
                DispatchQueue.main.async { [weak self] in
                    completion(newPosts, nil)
                }
                
            } catch{
                completion(nil,error)
            }
        })
        
        Task.resume()
    }
    
    // json 데이터 파일로 저장
    func writeJSON(_ items: [TC04Base64]) {

        do {
            let fileURL = try FileManager.default.url(for: .documentDirectory, in: .userDomainMask, appropriateFor: nil, create: true)
                            .appendingPathComponent("tc04_data.json")

            let encoder = JSONEncoder()
            try encoder.encode(items).write(to: fileURL)
            

        } catch {
            print(error.localizedDescription)
        }
    }
    
    // 파일경로
    func documentDirectoryPath() -> URL? {
        let path = FileManager.default.urls(for: .documentDirectory,
                                            in: .userDomainMask)
        return path.first
    }
    
    
    // URL로 이미지 다운로드
    func loadImage(fromURL url: URL,completion: @escaping (_ data: Data?, _ error: Error?) -> Void){
        
        //url에 정확한 이미지 url 주소를 넣는다.
//        guard let url = URL(string: ) else { return }
        //URLSession.shared.configuration.timeoutIntervalForRequest = 3600
        //URLSession.shared.configuration.timeoutIntervalForResource
        
        
        var urlRequest = URLRequest(url: url)
        urlRequest.httpMethod = "GET"
        urlRequest.timeoutInterval = TimeInterval(3600)
        urlRequest.httpBody = Data()

        let dataTask = URLSession.shared.dataTask(with: urlRequest, completionHandler: { data, response ,error in
        
            guard let data = data, error == nil,
                  let response = response as? HTTPURLResponse,
                  response.statusCode >= 200 && response.statusCode < 300
            else{
                completion(nil,error)
                return
            }
            
            let image = UIImage(data: data)
            
            //make file name from url
            let urlString = url.absoluteString
            let arr = urlString.components(separatedBy: "/")
            let fileName: String = arr[arr.count-1]
            
            // Save Image
            do{
                if let jpgData = image?.jpegData(compressionQuality: 0.5),
                   let path = self.documentDirectoryPath()?.appendingPathComponent(fileName) {
                    try? jpgData.write(to: path)
                    completion(data, nil)
                }
                
            }
            catch{
                completion(nil, error)
            }
            
        })
        dataTask.resume()
        
    }
    
    
    //이미지 읽어오기
    func readImage(named: String) -> UIImage? {
        if let dir: URL
          = try? FileManager.default.url(for: .documentDirectory,
                                         in: .userDomainMask,
                                         appropriateFor: nil,
                                         create: false) {
          let path: String
            = URL(fileURLWithPath: dir.absoluteString)
                .appendingPathComponent(named).path
          let image: UIImage? = UIImage(contentsOfFile: path)
          return image
//            readImagefile = nil
//            readImagefile = UIImage.init(contentsOfFile: path)
//            return readImagefile
        }
        return nil
    }
    
    // csv 파일 저장
    //func writeCSV(from recArray:[Dictionary<String, AnyObject>]) -> URL?{
    func writeCSV(from reusltCsv:String) -> URL?{
        let now = Date()
        
        //날짜시간으로 파일명 생성
        let date = DateFormatter()
        date.locale = Locale(identifier: "ko_kr")
        date.timeZone = TimeZone(abbreviation: "KST") // "2018-03-21 18:07:27"
        //date.timeZone = TimeZone(abbreviation: "NZST") // "2018-03-21 22:06:39"
        date.dateFormat = "yyyyMMdd-HHmmss"
        
        let csvFileName = "result_\(date.string(from: now))"
        
        var csvString = reusltCsv
        
        let filemanager = FileManager.default
        // csv 파일 저장
        let documents = filemanager.urls(for: .documentDirectory, in: .userDomainMask).first!
        
        let directoryURL = documents.appendingPathComponent("TestResults")
        
        //폴더생성
        do{
            try filemanager.createDirectory(atPath: directoryURL.path, withIntermediateDirectories: false, attributes: nil)
        }catch let e{
            print(e.localizedDescription)
        }
        
            
        // csv 파일 저장
        let path = directoryURL.appendingPathComponent("/\(csvFileName).csv")
        
        do {
            //let path = try fileManager.url(for: .documentDirectory, in: .allDomainsMask, appropriateFor: nil, create: false)
            //let fileURL = path.appendingPathComponent(csvFileName)
            try csvString.write(to: path, atomically: true, encoding: .utf8)
            return path
            
        } catch let error as NSError{
            print("Error creating File : \(error.localizedDescription)")
            return nil
        }


    }
    
    
    // csv 파일 읽어오기
    // 로컬 csv 파일을 읽어옴
    func readCSV(csvFileName: String) -> String{
        // FileManager 인스턴스 생성
        let fileManager: FileManager = FileManager.default
        // 사용자의 문서 경로
        let documentPath: URL = fileManager.urls(for: .documentDirectory, in: .userDomainMask)[0]
        
        
        // 파일을 저장할 디렉토리 경로(URL) 반환 (경로 추가)
        //let directoryPath: URL = documentPath.appendingPathComponent("새 폴더")
        
        let textPath: URL = documentPath.appendingPathComponent(csvFileName)
        
        // 만든 파일 불러와서 읽기.
        do {
            let dataFromPath: Data = try Data(contentsOf: textPath) // URL을 불러와서 Data타입으로 초기화
            let text: String = String(data: dataFromPath, encoding: .utf8) ?? "error" // Data to String
            return text
        } catch let e {
            print(e.localizedDescription)
            return "error"
        }
    }
    
    // url 에서 마지막에 (파일이름).(확장자) 를 반환
    func getFileNmae(urlString : String) -> String?{
        
        guard let url = URL(string: urlString) else { return nil}
        
        // Save Image
        let urlString = url.absoluteString
        let arr = urlString.components(separatedBy: "/")
        let fileName: String = arr[arr.count-1]
        
        return fileName
    }
    
}
