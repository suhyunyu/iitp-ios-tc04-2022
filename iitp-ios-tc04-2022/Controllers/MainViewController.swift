//
//  ViewController.swift
//  iitp-ios-tc04-2022
//
//  Created by lfin on 2022/04/26.
//

import UIKit
import UniformTypeIdentifiers
import MobileCoreServices


class MainViewController: UIViewController, UIDocumentPickerDelegate {
   
    @IBOutlet weak var NaviBar: UINavigationBar!
    
    @IBOutlet weak var stateLabel: UILabel!
    @IBOutlet weak var nextStepLabel: UILabel!
    @IBOutlet weak var queryImageLabel: UILabel!
    @IBOutlet weak var baseImageLabel: UILabel!
    
    @IBOutlet weak var LogTextview: UITextView!
    
    
    
    @IBOutlet weak var baseBorderView: UIView!
    @IBOutlet weak var queryBorderView: UIView!
    @IBOutlet weak var baseImageview: UIImageView!
    @IBOutlet weak var queryImageview: UIImageView!
    
    //@IBOutlet weak var loadDataButton: UIButton!
    @IBOutlet weak var loadDataButton: UIButton!
    @IBOutlet weak var testStartButton: UIButton!
    @IBOutlet weak var shareResultButton: UIButton!
    //@IBOutlet weak var testStartButton: UIButton!
    //@IBOutlet weak var shareResultButton: UIButton!
    
    let ttaDataManager : TTADataManager  = TTADataManager()
    let sqliteManager : SQLiteManager = SQLiteManager()
    //let moduleManager : NativeModuleManager = NativeModuleManager()
    
    var csvUrl : URL? = nil
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        //화면 꺼짐 방지
        UIApplication.shared.isIdleTimerDisabled = true
        
        
        NaviBar.layer.borderColor = UIColor.gray.cgColor
        NaviBar.layer.shadowColor = UIColor.gray.cgColor
        
        LogTextview.layer.cornerRadius = 10
        LogTextview.layer.borderWidth = 1
        LogTextview.layer.borderColor = UIColor.systemGray5.cgColor
        LogTextview.isEditable = false
        LogTextview.bounces = true

        
        loadDataButton.setTitle("데이터\n가져오기", for: .normal)
        loadDataButton.titleLabel?.textAlignment = .center
        loadDataButton.titleLabel?.lineBreakMode = .byWordWrapping
        loadDataButton.layer.cornerRadius = 5
        
        testStartButton.layer.cornerRadius = 5
        shareResultButton.layer.cornerRadius = 5
        
        //loadDataButton.titleLabel?.minimumScaleFactor = 0.5
        //loadDataButton.titleLabel?.adjustsFontSizeToFitWidth = true
        
        testStartButton.titleLabel?.textAlignment = .center
        testStartButton.titleLabel?.lineBreakMode = .byWordWrapping
        
        shareResultButton.setTitle("시험결과\n내보내기", for: .normal)
        shareResultButton.titleLabel?.textAlignment = .center
        shareResultButton.titleLabel?.lineBreakMode = .byWordWrapping
        
        
        //base 이미지 표시
        baseBorderView.layer.borderWidth = 1
        baseBorderView.layer.cornerRadius = 5
        baseBorderView.layer.borderColor = UIColor.gray.cgColor
        baseImageview.layer.cornerRadius = 5
        
        //query 이미지 표시
        queryBorderView.layer.borderWidth = 1
        queryBorderView.layer.cornerRadius = 5
        queryBorderView.layer.borderColor = UIColor.gray.cgColor
        queryImageview.layer.cornerRadius = 5
        


        //TestDataButton.setBackgroundColor(.blue, for: .normal)
        
        stateLabel.text = "초기화 중"
        
        //DB,Table 생성
        sqliteManager.createInfoTable()     //시험할 데이터 테이블 생성
        sqliteManager.createCountTable()    //시험횟수 테이블
        sqliteManager.deleteCountAll()
        sqliteManager.insertCountData(testCount: 0)
        print("Test count : \(sqliteManager.readCountData())")
        
        stateLabel.text = "초기화 완료"
        nextStepLabel.text  = "데이터 가져오기"
    
        
    }
    
    
    //버튼 다중클릭방지
    var loadDataAction = false
    // 데이터 가져오기
    @IBAction func onClickLoadData(_ sender: Any) {
        print("onClickLoadData start")
        if loadDataAction == true { return }
        loadDataAction = true
        
        stateLabel.text = "데이터 가져오는 중..."
        nextStepLabel.text  = "시험 시작"
        
        print("onClickLoadData!! \(loadDataAction)")
        //json 가져오기
        let baseUrl = "https://22tta.lfin.kr/data"
        guard let url = URL(string: baseUrl+"/json/queryPlan_v1.json") else { return }
        //guard let url = URL(string: baseUrl+"/json/queryPlan_v1_10000.json") else { return }
        
        baseImageview.image = nil
   
        ttaDataManager.loadImageJsonData(fromURL: url)
        { data, error in
            
            if let data = data?.items {
                
                // 모든 row 삭제
                self.sqliteManager.deleteInfoAll()
                
                for row in data{
                    //insert 이미지 데이터 정보
                    self.sqliteManager.insertData(tc04: row)
                }
                
            }
            else{
                DispatchQueue.main.async {
                    //데이터 오류 메시지
                    self.stateLabel.text = "json 데이터를 불러올 수 없습니다"
                    
                }
            }
            
            //이미지 다운로드
            let ttaTable = self.sqliteManager.readData()
            
            var imgList = Set<String>()     //다운로드할 이미지 목록 저장
            var downloadFailImgList = Set<String>() //다운로드 실패한 이미지 목록 저장
            
            //다운로드할 이미지 목록 생성
            for row in ttaTable{
                imgList.insert(row.bFileName)
                imgList.insert(row.qFileName)
            }
            //print(imgList)
            
            DispatchQueue.global().async { [weak self] in
                var i = 1 //실행 횟수
                var failCount = 0 //다운로드 실패한 카운트
                var loadCount = 1 //다운로드 성공한 이미지 카운트
                let rowCount = imgList.count
                
                let numberFormatter = NumberFormatter()
                numberFormatter.numberStyle = .decimal
                
                self?.ttaDataManager.removeImageFolder()
//                let ImageFileCount = self?.ttaDataManager.countImageFiles() ?? 0
//                print("Images Folder make empty(ImageFileCount : \(ImageFileCount))")
                
                
                
                for img in imgList{

                    // usleep(1000000) will sleep for 1 second
                    
                    guard let imageUrl = URL(string: "\(baseUrl)/images/tc04/\(img)") else { return }
                    self?.ttaDataManager.loadImage(fromURL : imageUrl){ data, error in
                        
                        
                        guard let data = data,error == nil else{
                            print("image download error: \(error)")
                            downloadFailImgList.insert(img)
                            return
                        }
                        
                        DispatchQueue.main.async {
                            i += 1
                            if let imageData = UIImage(data: data){
                                
                                //ImageFileCount = self?.ttaDataManager.countImageFiles() ?? 0
                                
                                var formatLoadCount = numberFormatter.string(from: NSNumber(value:loadCount))
                                var formatRowCount = numberFormatter.string(from: NSNumber(value:rowCount))
                                
                                print("이미지 파일 (\(loadCount)/\(rowCount)) 다운로드 중...")
                                
                                if(loadCount < rowCount){
                                    loadCount += 1
                                    self?.stateLabel.text = "이미지 파일 (\(formatLoadCount!)/\(formatRowCount!))\n다운로드 중..."
                                    self?.baseImageview.image = imageData
                                    
                                    
                                    let count = loadCount+downloadFailImgList.count
                                    if(count >= rowCount && downloadFailImgList.count != 0){
                                        let ImageFileCount = self?.ttaDataManager.countImageFiles() ?? 0
                                        
                                        formatLoadCount = numberFormatter.string(from: NSNumber(value:ImageFileCount))
                                        formatRowCount = numberFormatter.string(from: NSNumber(value:rowCount))
                                        print("image save fail:\(ImageFileCount)")
                                        self?.stateLabel.text = "데이터 준비 현황(\(formatLoadCount!)/\(formatRowCount!))"
                                        self?.nextStepLabel.text = "데이터 가져오기(재실행)"
                                        self?.baseImageview.image = nil
                                        self?.loadDataAction = false
                                        
                                    }
                                }
                                else if(loadCount == rowCount){
                                    self?.stateLabel.text = "데이터 준비 완료(\(formatLoadCount!)/\(formatRowCount!))"
                                    self?.loadDataButton.setTitle("데이터\n가져오기 ✅", for: .normal)
                                    self?.nextStepLabel.text = "시험 시작"
                                    self?.baseImageview.image = nil
                                    self?.loadDataAction = false
                                }
                                
                            }
                        }
                        

                    }
                    
                }
                
                
        
                
            }

            
            
        }

    }
        
 
    //다중 실행방지
    var startTestRunState = false
    //모듈 초기화
    let initVal = CVWrapper.initialize(0)
    
    // 시험시작
    @IBAction func onClickStarTest(_ sender: Any) {

        
        if startTestRunState == true { return }
        startTestRunState = true
        
        
        stateLabel.text = "시험 중.."
        LogTextview.text  = ""
        nextStepLabel.text = "시험시작"
        
        //데이터 테이블 조회
        let ttaTestTable = self.sqliteManager.readData()
        
        
        //데이터 테이블 조회
        let ttaTable = self.sqliteManager.readData()
        if ttaTable.count > 0 {
            //시작시간
            //let startTime = CFAbsoluteTimeGetCurrent()
            
            //모듈이 동작할 스레드
            DispatchQueue.global(qos: .userInitiated).async { [weak self] in
                
                //self?.ttaDataManager.clearCache()
                
                let statrState = CVWrapper.start()
                print("start \(statrState)")
                
                var statesMessage = "No Message"
                statesMessage = String(cString:CVWrapper.getPtrOfStringModule(0))
//                print("2 :: ",statesMessage)
                DispatchQueue.main.async { [weak self] in
                    self?.stateLabel.text = statesMessage
                }
                
                var rowCount = 0
                for row in ttaTable{
                    //반복문 1회 시행 시 마다 메모리를 비워줌
                    autoreleasepool{

                        guard let baseImage : UIImage = self?.ttaDataManager.readImage(named: row.bFileName) else{ return }
                        let baseImageVal = CVWrapper.putImageModule(baseImage)

                        // query 이미지 데이터 모듈로 전달
                        guard let queryImage : UIImage = self?.ttaDataManager.readImage(named: row.qFileName) else{ return }
                        let queryImageVal = CVWrapper.putImageModule(queryImage)

                        var statesMessage1 = String(cString:CVWrapper.getPtrOfStringModule(0))
                        //print("1 :: ",statesMessage1)
                        DispatchQueue.main.async { [weak self] in
                            self?.stateLabel.text = statesMessage1
                        }

                        
                        // meta data
                        var metaData = row.metadata

                        let decodedData = Data(base64Encoded: metaData)!  //base64 디코딩 (Base64 -> Data)
                        //let base64DecStr = String(data:decodedData, encoding: .utf8) //base64 디코딩 (Data -> String)


                        // 위치정보를 묶어서 전달
                        let byteVal = CVWrapper.putByteBlockModule(decodedData)

                        var statesMessage2 = String(cString:CVWrapper.getPtrOfStringModule(0))
                        //print("2 :: ",statesMessage2)
                        DispatchQueue.main.async { [weak self] in
                            self?.stateLabel.text = statesMessage2
                        }

                        // 데이터 계산
                        let processVal = CVWrapper.processModule()
                        var statesMessage3 = String(cString:CVWrapper.getPtrOfStringModule(0))
                        //print("3 :: ",statesMessage3)
                        DispatchQueue.main.async { [weak self] in
                            self?.stateLabel.text = statesMessage3
                        }

                        // 출력할 로그 데이터
                        let logString = String(cString:CVWrapper.getPtrOfStringModule(1))
                        print(logString)


                        DispatchQueue.main.async { [weak self] in

                            self?.baseImageview.image = self?.ttaDataManager.readImage(named: row.bFileName)
                            self?.baseImageLabel.text = row.bFileName
                            self?.queryImageview.image = self?.ttaDataManager.readImage(named: row.qFileName)
                            self?.queryImageLabel.text = row.qFileName

                            //로그 표시
                            self?.LogTextview.insertText(logString + "\n")
                            // 텍스트 추가시 스크롤 내리기
                            if(rowCount>5 && rowCount%2 == 0){
//                                let point = CGPoint(x: 0.0, y: ((self?.LogTextview.contentSize.height)! - (self?.LogTextview.bounds.height)!))
                                //self?.LogTextview.setContentOffset(point, animated: true)
                                
                                //텍스트 추가시 스크롤 내리기
                                if self?.LogTextview.text.count ?? 0 > 0 {
                                    let location = (self?.LogTextview.text.count ?? 1) - 1
                                    let bottom = NSMakeRange(location, 1)
                                    self?.LogTextview.scrollRangeToVisible(bottom)
                                }

                            }
                        }
                    }

                    rowCount += 1
                }
                    
                
                // 시험결과 csv파일 저장
                let resultCsv = String(cString:CVWrapper.getPtrOfStringModule(2))
                self?.csvUrl = self?.ttaDataManager.writeCSV(from: resultCsv)
                
                //thread 종료 시
                DispatchQueue.main.async { [weak self] in
                    
                    //[시험시작]버튼에 시험 횟수 표시
                    self?.sqliteManager.updateData()
                    var testCount = (self?.sqliteManager.readCountData())!
                    //self?.testStartButton.setTitle("시험시작\n(\(testCount!))", for: .normal)
                    
                    //진행상태
                    self?.stateLabel.text = "시험 완료"//"\(testCount)번째 시험 완료"
//                    self?.testStartButton.setTitle("시험시작", for: .normal)
                    
//                    if(testCount == 3){
                    //다음 지시사항
                    //self?.ttaDataManager.updateData(0)
                    self?.nextStepLabel.text = "시험결과 내보내기"
                    self?.testStartButton.setTitle("시험시작\n✅", for: .normal)
//                    }
                    
                    self?.baseImageview.image = nil
                    self?.baseImageLabel.text = ""
                    self?.queryImageview.image = nil
                    self?.queryImageLabel.text = ""
                    
                    //끝나는 시간
                    //let durationTime = CFAbsoluteTimeGetCurrent() - startTime
                    //print("경과 시간: \(durationTime)")
                    
                    
                }
                
                self?.startTestRunState = false
            }
            
        }
        else{
            stateLabel.text = "시험할 데이터가 없습니다"
            nextStepLabel.text = "데이터 가져오기"
            startTestRunState = false
        }
        
        //ttaDataManager.clearCache()
            
    }
    
    
    // 시험결과 내보내기
    @IBAction func onClickSahreTest(_ sender: UIBarButtonItem) {
        
        let documentPicker = UIDocumentPickerViewController(documentTypes: [kUTTypeData as String], in: .open)
        documentPicker.delegate = self
        documentPicker.allowsMultipleSelection = true
       
        
        let documents = FileManager.default.urls(for: .documentDirectory,
                                                 in: .userDomainMask).first!
        
        
        // Set the initial directory.
        documentPicker.directoryURL = documents.appendingPathComponent("TestResults")

        // Present the document picker.
        present(documentPicker, animated: true, completion: nil)
            
        
    }
    
    
    func documentPicker(_ controller: UIDocumentPickerViewController, didPickDocumentsAt urls: [URL]) {
        guard let myURL = urls.first else {
            print("ERROR : Unable to retrieve document.")
            return
        }
        
        // csv 파일 공유하기
        let activityViewController = UIActivityViewController(
            activityItems :  urls,
            applicationActivities: nil
        )

        if #available(iOS 15.0, *) {
            activityViewController.sheetPresentationController?.sourceView = self.shareResultButton
        } else {
            // Fallback on earlier versions
        }
        present(activityViewController, animated: true, completion: nil)
        
        
        activityViewController.completionWithItemsHandler = {(activityType: UIActivity.ActivityType?, completed: Bool, arrayReturnedItems: [Any]?, error: Error?) in
            if completed {
                self.stateLabel.text = "시험결과 내보내기 완료"
                self.shareResultButton.setTitle("시험결과\n내보내기\n✅", for: .normal)
                self.nextStepLabel.text = "앱 종료하기"
                //self.showToast(message: "share success")
                
            } else {
                self.stateLabel.text = "시험결과 내보내기 취소"
                //self.showToast(message: "share cancel")
            }
            if let shareError = error {
                self.showToast(message: "\(shareError.localizedDescription)")
                
            }
            
        }
    }
    
    
    // 토스트 메시지 출력
    func showToast(message : String) {
        let toastLabel = UILabel(frame: CGRect(x: self.view.frame.size.width/2 - 75, y: self.view.frame.size.height-100, width: 150, height: 35))
        toastLabel.backgroundColor = UIColor.black.withAlphaComponent(0.6)
        toastLabel.textColor = UIColor.white
        //toastLabel.font = font
        toastLabel.textAlignment = .center
        toastLabel.text = message
        toastLabel.alpha = 1.0
        toastLabel.layer.cornerRadius = 10
        toastLabel.clipsToBounds = true
        self.view.addSubview(toastLabel)
        UIView.animate(withDuration: 4.0, delay: 0.1, options: .curveEaseOut,animations: { toastLabel.alpha = 0.0 }, completion: {(isCompleted) in toastLabel.removeFromSuperview() })
        
    }
}

