//
//  TTAData.swift
//  iitp-ios-tc04-2022
//
//  Created by lfin on 2022/04/26.
//

import Foundation

struct TC04List: Codable{
    let items : [TC04Base64]
}

// 시험 Base 데이터
struct TC04Base64: Codable{
    let bFileName: String
    let qFileName: String
    let metadata: String
    
    enum CodingKeys: String, CodingKey {
        case bFileName = "b_file_name"
        case qFileName = "q_file_name"
        case metadata = "metadata"
    }
}
